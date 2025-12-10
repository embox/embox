## setup python's environment
### prepare default env
```bash
python3 -m venv path/to/venv
```
for example
```bash
python3 -m venv ~/ardupilot/venv
```
### activate env
```bash
source path/to/venv/bin/activate
```
for example
```bash
source ~/ardupilot/venv/bin/activate
```
### install prerequisite
move to embox folder
```bash
pip install -r project/ardupilot/req.txt
```

## Embox
### activate python env
```bash
source path/to/venv/bin/activate
```
### configure
```bash
make confload-project/ardupilot/arm_qemu
````
### build
```bash
make -j5
```
### run
```bash
./scripts/qemu/auto_qemu_with_sd_card
```


## Ardupilot sitl
### activate python env
```bash
source path/to/venv/bin/activate
```
### run mavproxy
```
mavproxy.py --out 10.0.2.16:14550 --master tcp:10.0.2.16:5760 --sitl 10.0.2.16:5501 --map --console
```

### commands
```
mode guided
arm throttle
takeoff 40
```
