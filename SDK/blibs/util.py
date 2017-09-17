
import sys, os, hashlib, datetime, platform, time, pickle,shutil
import subprocess

class systems:
	def __init__(self, tag):
		self.__tag = tag

	@staticmethod
	def current():
		if platform.system() == 'Windows':
			return systems.win32
		if platform.system() == 'Linux':
			return systems.linux
		return systems.unknown

	def __str__(self):
		if self == systems.win32:
			return 'nt'
		if self == systems.linux:
			return 'linux'
		return 'unknown'
	
	def __repr__(self):
		if self == systems.win32:
			return 'nt'
		if self == systems.linux:
			return 'linux'
		return 'unknown'

systems.unknown = systems('system.unknown')
systems.win32 = systems('system.win32')
systems.linux = systems('system.linux')

class batch_command:
	def __init__( self, working_dir, target_sys = systems.current() ):
		self.dir_ = working_dir
		self.commands_ = []
		
		# Environ setup
		if target_sys == systems.win32:
			self.execute_template_ = "%s"
			self.file_suffix_ = ".bat"
			self.error_exit_template_ = \
				"@%s\n" + \
				"@if %%ERRORLEVEL%% neq 0 exit /B 1"
		elif target_sys == systems.linux:
			self.execute_template_ = "sh %s"
			self.file_suffix_ = ".sh"
			self.error_exit_template_ = \
				"%s || exit $?"
		else:
			report_error("OS is unsupported by batch_command.")
		
	def add_native_command(self, cmd):
		self.commands_ += [cmd]
	
	def add_execmd(self, cmd):
		self.commands_ += [cmd]
		
	def add_execmd_with_error_exit(self, cmd):
		self.commands_ += [self.error_exit_template_ % cmd]
	
	def execute(self, keep_bat = False):
		tmp_gen = hashlib.md5()
		dt = datetime.datetime.now()
		tmp_gen.update( str(dt).encode('utf-8') )
		batch_fname = tmp_gen.hexdigest() + self.file_suffix_
		curdir = os.path.abspath(os.curdir)
		#print(curdir)
		#print(self.dir_)
		os.chdir(self.dir_)
		batch_f = open(batch_fname, "w")
		batch_f.writelines( [cmd_line + "\n" for cmd_line in self.commands_] )
		batch_f.close()
		ret_code = os.system(self.execute_template_ % batch_fname)
		if not keep_bat:
			os.remove(batch_fname)
		os.chdir(curdir)
		return ret_code

def get_git_revision_hash():
	#return subprocess.check_output(['git', 'rev-parse', 'HEAD'])  # need python 2.7
	return subprocess.Popen(['git', 'rev-parse', 'HEAD'], stdout=subprocess.PIPE).communicate()[0]

def get_git_revision_short_hash():
	return subprocess.Popen(['git', 'rev-parse', '--short', 'HEAD'], stdout=subprocess.PIPE).communicate()[0]
	#return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])

def generate_version(tmpl_path):
	path = os.path.abspath(tmpl_path)
	version_name, ext = os.path.splitext(path)
	if os.path.exists(version_name):
		os.remove(version_name)
	shutil.copy2(path, version_name)
	# get git revision
	rev_hash = get_git_revision_hash().strip()
	short_hash = get_git_revision_short_hash().strip()
	# write version file
	lines = open(version_name, 'r').readlines()
	flen = len(lines)
	for i in range(flen):
		if "$FULL_VERSION" in lines[i]:
			lines[i] = lines[i].replace("$FULL_VERSION", rev_hash)
		if "$SHORT_VERSION" in lines[i]:
			lines[i] = lines[i].replace("$SHORT_VERSION", short_hash)
		if "$BUILD_TIME" in lines[i]:
			tm = time.strftime("%m%d_%H%M", time.localtime(time.time()))
			lines[i] = lines[i].replace("$BUILD_TIME", tm)
	open(version_name, 'w').writelines(lines)
	
