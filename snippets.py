# -*- coding: utf-8 -*-
import os
import hashlib
import json
from multiprocessing import Process


def cal_md5(filename):
    if not os.path.isfile(filename):
        print(filename+" is not a regular file")
        return ""
    else:
        f = open(filename,'r')
        md5str = hashlib.md5(f.read().encode("utf8")).hexdigest()
        f.close()
        return md5str


"""
    {
        "/etc/hosts":{
        
        "md5":"",
        "effected_service":["",""]
        },
        
    }
"""

fileLists = ["/etc/hosts","/etc/networks"]
fileMap = {}


def cal_md5_file_list():
    for filename in fileLists:
        fileMap[filename] = {}
        fileMap[filename]['md5'] = cal_md5(filename)
        fileMap[filename]['effected_service'] = ["1","2"]
    return json.dumps(fileMap)



if __name__ == '__main__':
    f = open("./md5lists.json",'w')
    f.write(cal_md5_file_list())
    f.close()
    f = open("./md5lists.json","r")
    tmp = json.loads(f.read())
    tmp['/etc/hosts']['md5'] = 1
    print(tmp['/etc/hosts']['md5'])


