from distutils.core import setup
setup(name='vpi',
      version='1.0.0',
      description='Python VPI Binding',
      author='Support',
      author_email='support@bigswitch.com',
      packages=['vpi'],
      data_files=[('/usr/bin', ['vpi/pyvpitool']), ('/usr/lib', ['build/gcc-local/bin/pyvpi.so'])]
      )
