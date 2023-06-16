required_plugins = %w(vagrant-reload)

required_plugins.each do |plugin|
  puts "#{plugin} not available, run `vagrant plugin install #{plugin}' (affects all vagrants on system)" unless Vagrant.has_plugin? plugin
end

Vagrant.configure("2") do |config|
  config.vm.network "forwarded_port", guest: 80, host: 8080
  config.vm.network "forwarded_port", guest: 1234, host: 1234

  config.vm.provider "virtualbox" do |v|
    v.memory = 2048
    v.cpus = 1
  end
  
  config.vm.define "xen_alpine", autostart: false do |xen|
    xen.vm.box = "bento/ubuntu-16.04"
    
    xen.vm.synced_folder ".", "/embox", type: "rsync",
	    rsync__exclude: ".git/"

    xen.vm.provision "shell", inline: <<-SHELL
      export DEBIAN_FRONTEND=noninteractive
      apt-get update
      apt-get -y upgrade
      apt-get -y install xen-system-amd64 bridge-utils httperf

      echo "cd /embox" >> /home/vagrant/.bashrc
      echo "export PATH=$PATH:/usr/lib/xen-4.6/bin" >> /home/vagrant/.bashrc
    SHELL

    xen.vm.provision :reload
  end

  config.vm.define "xen_debug", autostart: false do |xen|
    xen.vm.box = "ubuntu/bionic64"
    
    xen.vm.synced_folder ".", "/embox", type: "rsync",
	    rsync__exclude: ".git/"

    xen.vm.provision "shell", inline: <<-SHELL
      export DEBIAN_FRONTEND=noninteractive
      apt-get update
      apt-get -y upgrade
      apt-get -y install xen-system-amd64 bridge-utils httperf

      echo "cd /embox" >> /home/vagrant/.bashrc
      echo "export PATH=$PATH:/usr/lib/xen-4.9/bin" >> /home/vagrant/.bashrc
    SHELL

    xen.vm.provision :reload
  end
end
