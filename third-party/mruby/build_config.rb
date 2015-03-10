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

  conf.linker.command = ENV['CROSS_LD']
  conf.linker.flags = ENV['CROSS_LDFLAGS']
  conf.archiver.command = ENV['CROSS_AR']

  #conf.bins = %w(mrbc mruby mirb)
  #conf.bins = %w(mrbc)

  #do not build executable test
  #conf.build_mrbtest_lib_only

  #disable C++ exception
  #conf.disable_cxx_exception

  #gems from core
  conf.gem :core => 'mruby-bin-mirb'
  conf.gem :core => 'mruby-bin-mruby'

  conf.gem :core => "mruby-print"
  #conf.gem :core => "mruby-sprintf"
  #conf.gem :core => "mruby-math"
  #conf.gem :core => "mruby-enum-ext"

  #light-weight regular expression
  #conf.gem :github => "masamitsu-murase/mruby-hs-regexp", :branch => "master"

end

