
required_plugins = %w(vagrant-reload)

required_plugins.each do |plugin|
  puts "#{plugin} not available, run `vagrant plugin install #{plugin}' (affects all vagrants on system)" unless Vagrant.has_plugin? plugin
end

Vagrant.configure("2") do |config|

  config.vm.define "xen", autostart: false do |xen|
    xen.vm.box = "ubuntu/xenial64"

    xen.vm.provision "shell", inline: <<-SHELL
    DEBIAN_FRONTEND=noninteractive \
	  apt-get -y update
    DEBIAN_FRONTEND=noninteractive \
	  apt-get -y install \
		  xen-hypervisor-amd64

    echo "cd /embox" >> /home/ubuntu/.bashrc
    SHELL

    xen.vm.provision :reload

    xen.vm.synced_folder ".", "/embox", type: "rsync",
	    rsync__exclude: ".git/"
  end

end
