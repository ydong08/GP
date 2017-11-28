
import sys, os, shutil

def copy_dfff(src, dst):
    pass

def copy_newer(src, dst):
    if not (os.path.isfile(src)):
        print "error: src parameter of copy newer is a file path."
    
    target = None

    if os.path.isdir(dst):
        target = os.path.join(dst, os.path.basename(src))
    else:
        target = dst
    
    if os.path.exists(target):
        if os.path.getmtime(target) < os.path.getmtime(src):
            os.remove(target)

    if not os.path.exists(target):
        shutil.copy2(src, target)
        return True
    else:
        return False