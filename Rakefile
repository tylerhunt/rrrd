require 'rubygems'
require 'rake/clean'
require 'rake/gempackagetask'
require 'rake/rdoctask'
require 'spec/rake/spectask'

desc 'Default: package gem.'
task :default => :spec

spec = Gem::Specification.new do |spec|
  spec.name = 'rrrd'
  spec.version = '0.0.1'
  spec.summary = 'A Ruby interface for RRD.'
  spec.author = 'Tyler Hunt'
  spec.email = 'tyler@tylerhunt.com'
  spec.homepage = 'http://tylerhunt.com/'
  spec.rubyforge_project = 'rrrd'
  spec.platform = Gem::Platform::RUBY
  spec.files = %w(README.markdown LICENSE Rakefile) + Dir.glob("{lib,spec}/**/*") + Dir.glob("ext/**/*.{h,c,rb}")
  spec.require_path = 'lib'
  spec.extensions = FileList["ext/**/extconf.rb"].to_a
  spec.test_files = FileList['spec/**/*spec.rb'].to_a
  spec.has_rdoc = true
  spec.extra_rdoc_files = ['README.rdoc', 'LICENSE']
end

Rake::GemPackageTask.new(spec) do |package|
  package.need_tar = true
end

Rake::RDocTask.new do |rdoc|
  rdoc.title    = 'RRRD Documentation'
  rdoc.main     = 'README.rdoc'
  rdoc.rdoc_dir = 'rdoc'
  rdoc.rdoc_files.include('README', 'LICENSE', 'lib/**/*.rb')
  rdoc.options << '--inline-source'
  rdoc.options << '--line-numbers'
end

Spec::Rake::SpecTask.new(:spec) do |t|
  t.spec_opts   = ['--colour --format progress --loadby mtime --reverse']
  t.spec_files  = FileList['spec/**/*_spec.rb']
end

Spec::Rake::SpecTask.new(:doc) do |t|
  t.spec_opts   = ['--format specdoc --dry-run --colour']
  t.spec_files  = FileList['spec/**/*_spec.rb']
end

desc "Builds native extensions"
task :build => ["ext/rrrd/Makefile", "ext/rrrd/rrrd.so"]

file "ext/rrrd/Makefile" => ["ext/rrrd/extconf.rb"] do
  Dir.chdir("ext/rrrd") do
    ruby "extconf.rb"
  end
end

file "ext/rrrd/rrrd.so" => Dir.glob('ext/rrrd/{*.{c,h},extconf.rb,Makefile}') do
  Dir.chdir("ext/rrrd") do
    sh("make")
  end

  mkdir_p "lib"
  cp "ext/rrrd/rrrd.so", "lib"
end
