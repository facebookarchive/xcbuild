import os
import subprocess


def FlagsForFile(filename):
  flags = []

  root_dir = os.path.normpath(os.path.dirname(filename))
  while not (os.path.exists(os.path.join(root_dir, 'Sources'))):
    next_dir = os.path.normpath(os.path.join(root_dir, '..'))
    if next_dir == root_dir:
      return flags
    root_dir = next_dir

  if filename.endswith('.h'):
    for alt_ext in ['.cpp', '.cc']:
        for alt_dir in ['Headers', 'PrivateHeaders']:
          alt_name = filename[:-2] + alt_ext
          alt_name = alt_name.replace(alt_dir, 'Sources')
          if os.path.exists(alt_name):
            filename = alt_name
            break

  build_dir = os.path.join(root_dir, 'build')
  ninja_filename = os.path.join('..', filename[len(root_dir) + 1:])
  p = subprocess.Popen(['ninja', '-v', '-C', build_dir, '-t', 'commands', ninja_filename + '^'],
                       stdout=subprocess.PIPE)
  stdout, stderr = p.communicate()
  if p.returncode:
    return flags

  clang_line = None
  for line in reversed(stdout.split('\n')):
    if 'c++' in line or 'clang++' in line:
      clang_line = line
      break
  else:
    return flags

  for flag in clang_line.split(' '):
    if flag.startswith('-I'):
      if flag[2] == '/':
        flags.append(flag)
      else:
        abs_path = os.path.normpath(os.path.join(build_dir, flag[2:]))
        flags.append('-I' + abs_path)
    elif flag.startswith('-std'):
      flags.append(flag)
    elif flag.startswith('-') and flag[1] in 'DWFfmO':
      flags.append(flag)

  return {
      'flags': flags,
      'do_cache': True
  }
