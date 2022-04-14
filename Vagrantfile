Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/xenial32"

  config.vm.box_check_update = false

  config.vm.provider "virtualbox" do |vb|
    vb.name = "embox-lkl"
    vb.memory = "8096"
    vb.cpus = 4
  end

  config.vm.synced_folder ".", "/embox", type: "rsync"

  # Enable provisioning with a shell script
  config.vm.provision "shell", inline: <<-SHELL
    apt-get update
    apt-get install -y git gdb qemu-system-i386 build-essential gcc-multilib curl libmpc-dev python zip libfuse-dev libarchive-dev xfsprogs bison flex bc
  SHELL
end
