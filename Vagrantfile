required_plugins = %w(vagrant-reload)

required_plugins.each do |plugin|
  puts "#{plugin} not available, run `vagrant plugin install #{plugin}' (affects all vagrants on system)" unless Vagrant.has_plugin? plugin
end

Vagrant.configure("2") do |config|
  config.vm.network "forwarded_port", guest: 80, host: 8080

  config.vm.define "xen", autostart: false do |xen|
    xen.vm.box = "debian/stretch64"
    
    xen.vm.synced_folder ".", "/embox", type: "rsync",
	    rsync__exclude: ".git/"

    xen.vm.provision "shell", inline: <<-SHELL
      export DEBIAN_FRONTEND=noninteractive
      apt-get update
      apt-get -y upgrade
      apt-get -y install xen-system-amd64 bridge-utils

      echo "cd /embox" >> /home/vagrant/.bashrc
      echo "export PATH=$PATH:/usr/lib/xen-4.8/bin" >> /home/vagrant/.bashrc
      cat /embox/scripts/xen/interfaces > /etc/network/interfaces
    SHELL

    xen.vm.provision :reload
  end
end
