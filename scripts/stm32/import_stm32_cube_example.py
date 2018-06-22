#!/usr/bin/env python2

import sys
import os
import argparse
import shutil
import subprocess
import textwrap

class ExampleInfo:
	def __init__(self, src, dst, script_dir, example_name, platform):
		self.src = src
		self.dst = dst
		self.script_dir = script_dir
		self.example_name = example_name
		self.platform = platform

# We want to obtain something like this:
#
# ...
# extern int embox_stm32_setup_irq_handlers(void);
# int BSP_main(void){
# 	uint8_t  lcd_status = LCD_OK;
#
# 	if (0 != embox_stm32_setup_irq_handlers()) {
# 		printf("embox_stm32_setup_irq_handlers error!\n");
# 	}
# 	//CPU_CACHE_Enable();
# 	//HAL_Init();
# 	//SystemClock_Config();
# ...
def fix_main(info):
	with open(info.dst + '/Src/main.c', 'r') as f:
		lines = f.readlines()

	def find_line(pattern):
		l = [i for i in range(len(lines)) if pattern in lines[i]]
		return l[0] if l else None

	def comment_line_out_if_any(pattern):
		id = find_line(pattern)
		if id is not None:
			lines[id] = '  //' + lines[id]

	id = find_line('main(void)')
	if id is not None:
		lines[id] = 'int %s_main(void)' % info.example_name
		lines.insert(id, 'extern int embox_stm32_setup_irq_handlers(void);\r\n')

	id = find_line('HAL_Init();')
	if id is not None:
		lines.insert(id,
					('  if (0 != embox_stm32_setup_irq_handlers()) {\r\n'
					 '    printf("embox_stm32_setup_irq_handlers error!\\n");\r\n'
					 '  }\r\n'))

	comment_line_out_if_any('CPU_CACHE_Enable();')
	comment_line_out_if_any('HAL_Init();')
	comment_line_out_if_any('SystemClock_Config();')

	with open(info.dst + '/Src/main.c', 'w') as f:
		f.write(''.join(lines))

def generate_Mybuild(info):
	with open(info.script_dir + '/Mybuild_template', 'r') as f:
		mybuild = f.read()

	mybuild = mybuild.replace('_EXAMPLE_', info.example_name)
	mybuild = mybuild.replace('_PLATFORM_', info.platform)

	sources = 'source'
	for file in os.listdir(info.src + '/Src'):
		if file.endswith('.c'):
			sources += '		"Src/%s",\n' % file
	sources += '		"Src/embox_stm32%sxx_it.c"\n' % info.platform
	mybuild = mybuild.replace('_SOURCES_', sources)

	with open(info.dst + '/Mybuild', 'w') as f:
		f.write(mybuild)

def find_irq_handlers_in_file(info, file):
	# Get path of STM32 Cube directory
	stm32_path = info.src.rsplit('/Projects', 1)[0]

	# First of all, we need to preprocess file
	compiler = 'arm-none-eabi-cpp'
	include = {'f7' :
				' -DSTM32F746xx'											+ \
				' -I_SRC_/Inc'												+ \
				' -I_STM32_PATH_/Drivers/STM32F7xx_HAL_Driver/Inc'			+ \
				' -I_STM32_PATH_/Drivers/BSP/STM32746G-Discovery'			+ \
				' -I_STM32_PATH_/Drivers/CMSIS/Device/ST/STM32F7xx/Include'	+ \
				' -I_STM32_PATH_/Drivers/CMSIS/Include'						+ \
				' -I_STM32_PATH_/Utilities/Log'								+ \
				' -I_STM32_PATH_/Middlewares/Third_Party/FatFs/src'			+ \
				' -I_STM32_PATH_/Middlewares/ST/STM32_USB_Device_Library/Core/Inc' + \
				' -I_STM32_PATH_/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc' + \
				' -I_STM32_PATH_/Middlewares/ST/STM32_USB_Host_Library/Core/Inc' + \
				' -I_STM32_PATH_/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc',
				'f4' :
				' -DSTM32F407xx'											+ \
				' -I_SRC_/Inc'												+ \
				' -I_STM32_PATH_/Drivers/STM32F4xx_HAL_Driver/Inc'			+ \
				' -I_STM32_PATH_/Drivers/BSP/STM32F4-Discovery'				+ \
				' -I_STM32_PATH_/Drivers/CMSIS/Device/ST/STM32F4xx/Include'	+ \
				' -I_STM32_PATH_/Drivers/CMSIS/Include'}[info.platform]
	include = include.replace('_SRC_', info.src)
	include = include.replace('_STM32_PATH_', stm32_path)
	# Compile and find implementations of IRQHandler's, not declarations
	command = compiler + ' ' + include + ' ' + file + ' | grep "IRQHandler(void)" | grep -v ";"'

	# Search for 'IRQHandler' in the obtained preprocessed file
	p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
	result = [w for w in p.stdout.read().replace('(', ' ').replace(')', ' ').split() if "IRQHandler" in w]

	return result

def generate_stm32_irq_handlers(info):
	stm32_it_file = info.src + '/Src/stm32%sxx_it.c' % info.platform
	irq_handlers = find_irq_handlers_in_file(info, stm32_it_file)

	with open(info.script_dir + '/embox_stm32_it_template.c', 'r') as f:
		stm32_it = f.read()

	stm32_it = stm32_it.replace('_PLATFORM_', info.platform)

	irq_attach = ''
	irq_handlers_declare = ''
	for h in irq_handlers:
		prefix = h.rsplit('_IRQHandler', 1)[0]
		irq_attach += '	res |= irq_attach(%s_IRQn + 16, embox_%s, 0, NULL, "%s");\n' % (prefix, h, h)
		irq_handlers_declare += 'EMBOX_STM32_IRQ_HANDLER(%s)\n' % prefix

	stm32_it = stm32_it.replace('_IRQ_HANDLERS_DECLARE_', irq_handlers_declare)
	stm32_it = stm32_it.replace('_STM32_IRQ_ATTACH_', irq_attach)

	with open(info.dst + '/Src/embox_stm32%sxx_it.c' % info.platform, 'w') as f:
		f.write(stm32_it)

def generate_new_main(info):
	with open(info.script_dir + '/embox_main_template.c', 'r') as f:
		embox_main = f.read()

	embox_main = embox_main.replace('_EXAMPLE_', info.example_name)

	with open(info.dst + '/Src/embox_main.c', 'w') as f:
		f.write(embox_main)

def import_example(info):
	if os.path.exists(info.dst):
		print 'Destination directory %s is already exists' % info.dst
		return
	elif not os.path.exists(info.src):
		print 'Source directory %s does not exist' % info.src
		return

	shutil.copytree(info.src + '/Src', info.dst + '/Src')
	shutil.copytree(info.src + '/Inc', info.dst + '/Inc')
	shutil.copyfile(info.src + '/readme.txt', info.dst + '/readme.txt')

	fix_main(info)
	generate_Mybuild(info)
	generate_stm32_irq_handlers(info)
	generate_new_main(info)

def main():
	parser = argparse.ArgumentParser(
		prog='import_stm32_cube_example.py',
		formatter_class=argparse.RawDescriptionHelpFormatter,
		description=textwrap.dedent('''\
			Example of use
			--------------------------------
			Do the following to import LTDC_Display_1Layer example:
			# ./scripts/stm32/import_stm32_cube_example.py --name LTDC_Display_1Layer --platform f7
			<path to>/STM32Cube_FW_F7_V1.5.0/Projects/STM32746G-Discovery/Examples/LTDC/LTDC_Display_1Layer
			''')
	)
	parser.add_argument('--platform', help='f4 or f7')
	parser.add_argument('src', help='Source folder containing Cube example')
	parser.add_argument('dest', nargs='?', default='', help='Destination folder '
						'(./platform/stm32<platform>/cmds by default)')
	parser.add_argument('--name', default='', help='Imported example name '
						'(corresponds to Cube\'s example name by default)')
	args = parser.parse_args()

	platform = args.platform

	if args.dest == '':
		args.dest = os.getcwd() + '/platform/stm32%s/cmds' % platform

	src = os.path.normpath(args.src)

	if args.name == '':
		example_name = os.path.basename(src)
	else:
		example_name = args.name

	dst = '%s/%s' % (os.path.normpath(args.dest), example_name)
	script_dir = os.path.dirname(sys.argv[0])

	print "src=%s, dest=%s, platform=%s" % (src, dst, platform)
	info = ExampleInfo(src, dst, script_dir, example_name, platform)

	import_example(info)

if __name__ == "__main__":
	main()
