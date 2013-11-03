require 'rubygems'
require 'rbconfig'

puts "== Executing as '#{ENV['USER']}' =="

###############################
# BUILD CONFIGURATION VARIABLES
###############################

# Specify the build target - Debug, Release, RelMinSize, RelWithDebug
$buildTarget="Debug" # "Debug"

namespace :build do

desc "Builds OS X"

  task :osx do

      puts "== Building OS X =="
      FileUtils.mkdir_p("build/cmake_osx")    
      Dir.chdir("build/cmake_osx") do
        sh "cmake ../../ -G \"Unix Makefiles\" -DCMAKE_BUILD_TYPE=#{$buildTarget}"
        sh "make -j4"
      end
      
  end

end