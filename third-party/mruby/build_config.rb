MRuby::Build.new do |conf|

  toolchain :gcc
  # include the default GEMs
  conf.gembox 'default'

  conf.cc do |cc|
     cc.command = ENV['HOST_CC'] || 'gcc'
     cc.flags = (ENV['HOST_CFLAGS'] || %w(-g -O3 -Wall -Werror-implicit-function-declaration))
  end

  conf.linker do |linker|
    linker.command = ENV['HOST_LD'] || 'gcc'
    linker.flags = [ENV['HOST_LDFLAGS'] || %w(-lm)]
  end

  conf.archiver do |archiver|
     archiver.command = ENV['AR'] || 'ar'
  end
end

MRuby::CrossBuild.new('embox') do |conf|

  conf.cc.command = ENV['CROSS_CC']
  conf.cc.flags << ENV['CROSS_CFLAGS']

  conf.linker.command = ENV['CROSS_CC']
  conf.archiver.command = ENV['CROSS_CC']

end

