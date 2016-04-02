import os
import commands

result = ""

for root, dirs, files in os.walk("../hidden_testcase/"):
	for dir in dirs:
		os.system('cp ../hidden_testcase/%s/dimage.bin dimage.bin'%(dir))
		os.system('cp ../hidden_testcase/%s/iimage.bin iimage.bin'%(dir))
		os.system('./single_cycle')
		status, result1 = commands.getstatusoutput('diff ../hidden_testcase/%s/snapshot.rpt snapshot.rpt'%(dir))
		status, result2 = commands.getstatusoutput('diff ../hidden_testcase/%s/error_dump.rpt error_dump.rpt'%(dir))
		if result1 != "" or result2 != "":
			result += 'fail in hidden_testcase ' + dir + '\n'
	break

for root, dirs, files in os.walk("../Student_valid_testcase/"):
	for dir in dirs:
		print dir
		os.system('cp ../Student_valid_testcase/%s/dimage.bin dimage.bin'%(dir))
		os.system('cp ../Student_valid_testcase/%s/iimage.bin iimage.bin'%(dir))
		os.system('./single_cycle')
		status, result1 = commands.getstatusoutput('diff ../Student_valid_testcase/%s/snapshot.rpt snapshot.rpt'%(dir))
		status, result2 = commands.getstatusoutput('diff ../Student_valid_testcase/%s/error_dump.rpt error_dump.rpt'%(dir))
		if result1 != "" or result2 != "":
			result += 'fail in ' + dir + ' test\n'
	break
if result == "":
	print 'pass all'
else:
	print result
