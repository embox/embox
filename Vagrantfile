
required_plugins = %w(vagrant-reload)

required_plugins.each do |plugin|
  puts "#{plugin} not available, run `vagrant plugin install #{plugin}' (affects all vagrants on system)" unless Vagrant.has_plugin? plugin
end

Vagrant.configure("2") do |config|

  config.vm.define "xen", autostart: false do |xen|
    xen.vm.box = "ubuntu/bionic64"

    xen.vm.provision "shell", inline: <<-SHELL
    DEBIAN_FRONTEND=noninteractive \
	  apt-get -y update
    DEBIAN_FRONTEND=noninteractive \
	  apt-get -y install \
		  xen-hypervisor-amd64

    echo "cd /embox" >> /home/vagrant/.bashrc
    echo "export PATH=$PATH:/usr/lib/xen-4.9/bin" >> /home/vagrant/.bashrc
    SHELL

    xen.vm.provision :reload

    xen.vm.synced_folder ".", "/embox", type: "rsync",
	    rsync__exclude: ".git/"

  end

  config.vm.network "forwarded_port", guest: 1234, host: 1234
end
