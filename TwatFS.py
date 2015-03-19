#TwatFS alpha 0.4 TwatFS@dc949.org by dc949
#Add argument support
#add failed file support
#add config file for authentication

import twatty, utflib, string, time, base64, bz2, zlib, math, subprocess

###########################
username = ''
password = ''
timeout = 0
failtimeout = 3
retrylimit=5; 
netassume = 1.5 # assumes n seconds for each http request, lame guesing ETC
decode_test = 0 #
send_twats = 1 #debug mode

ignore_broken_files = 1 #Ignore files that are missing chunks in the filestream during download
######################
use_utf = 1
chunk_len = (140.0,560.0)[use_utf]
verbose = 1
######################


def twat(conf, status, failout=0):
  if failout >= retrylimit: return 0
  conf['data']['status'] = status
  conf['url'] = 'https://twitter.com/statuses/update.xml'

  check = twatty.request(conf)
  if check is 0:
    time.sleep(failtimeout)
  else:
    print '.',

def write(fn, data, wt='wb'):
  f = open(fn, wt)
  f.write(data)
  f.close()

def encode_base64(data):
  return base64.encodestring(data)

def decode_base64(data):
  return base64.decodestring(data)

def compress_bz2(data):
  return str(bz2.compress(data))

def decompress_bz2(data):
  return bz2.decompress(data)

def compress_gzip(data):
  return zlib.compress(data,9)

def decompress_gzip(data):
  return zlib.decompress(data)

def recomend_compression(data):
  compressed_bz2 = compress_bz2(data)
  compressed_gzip = compress_gzip(data)
  if use_utf:
    cppHandle = subprocess.Popen('', stdin=subprocess.PIPE, shell=False,
				executable="/home/adam/TwatFS/trunk/TwatFS/utfsmuggler",
				stdout=subprocess.PIPE)
    datbz2  = cppHandle.communicate(compressed_bz2)[0]
    cppHandle = subprocess.Popen('', stdin=subprocess.PIPE, shell=False,
				executable="/home/adam/TwatFS/trunk/TwatFS/utfsmuggler",
				stdout=subprocess.PIPE)
    datgzip = cppHandle.communicate(compressed_gzip)[0]
    #    datbz2  = utflib.encode_utf(compressed_bz2)
    #    datgzip = utflib.encode_utf(compressed_gzip)
  else:
    datbz2  = encode_base64(compressed_bz2)
    datgzip = encode_base64(compressed_gzip)
  
  scorebz2  = len(datbz2)
  scoregzip = len(datgzip)
  
  if verbose: #like mortuary
    print "\n== Compression Information =="
    print "raw len:       %s" % len(data)
    print "Encoding Type: %s" % ("base64","utf8")[use_utf]
    print "Chunk Length:  %s" % chunk_len

    print "bz2 len:       %s :  %s Chunks" % (scorebz2, str(int(math.ceil(scorebz2/chunk_len))))
    print "gzip len:      %s :  %s Chunks" % (scoregzip, str(int(math.ceil(scoregzip/chunk_len))))

    print "Recomended compression method:",("gzip","bz2")[scorebz2<scoregzip],"\n"
  return (['gzip', datgzip],['bz2', datbz2])[scorebz2<scoregzip]
  
def main_up():
  conf = {}
  conf['auth'] = {}
  conf['data'] = {}
  conf['auth']['user'] = username
  conf['auth']['passwd'] = password

  filelocation=raw_input("Please enter the filename: ")
  
  filename=(filelocation,filelocation[filelocation.rfind("/")+1:])['/' in filelocation]
  try:
     data = open(filelocation, 'rb').read()
  except IOError,(errno):
     print 'Could not open file:',errno,"\nQuiting"
     return 0

  meta = 'file='+filename+';'
  
  compressed = recomend_compression(data)
  comptype = compressed[0]
  data = compressed[1]
  meta += 'comp='+comptype+';'

  chunks = int(math.ceil(len(data)/chunk_len))

  meta += 'chunks=' + str(chunks)+';'
  if use_utf: meta+='enc=utf8;'
  else: meta+='enc=base64;'
  
  if decode_test:
    cppHandle = subprocess.Popen('-d', stdin=subprocess.PIPE, shell=False,
				executable="/home/adam/TwatFS/trunk/TwatFS/utfsmuggler",
				stdout=subprocess.PIPE)
    decodedData = cppHandle.communicate(data)[0]
    write('_'+filename, eval('decompress_'+comptype)(decodedData)) #assumes use_utf
    #write('_'+filename, eval('decompress_'+comptype)(utflib.decode_utf(data))) #assumes use_utf
   
  if verbose:
    print "Meta: %s\n"%meta

  print "Filename: %s"%filename
  print "This file requires %s twats." % str(chunks+2)
  if(chunks+2) >= 99:
    print "Warning, this file requires 100 Tweets or more, rate limiting is a bitch.\nIf you still want to upload the file, its recomended you set timeout to something rather high."
  print "That will take aprox %s+ seconds"% str((chunks*timeout)+(int(chunks*netassume)))
  cont = raw_input("Continue? ")
  if cont in['yes','y','1']:
    if send_twats: #debug
      twat(conf, 'TwatFS=start;'+meta) #needs length checking for 140 char restriction
      time.sleep(timeout)
      while len(data):
        bytes = 0
        chars = 0
        # keep reading until we have 140 char or we're out of data to read
        while chars < 140 and bytes < len(data):
          if data[bytes] >= 0xF0:
            bytes += 4
          elif data[bytes] >= 0xD0:
            bytes += 3
          elif data[bytes] >= 0xC0:
            bytes += 2
          else:
            bytes += 1
          chars += 1

        twat(conf, data[0:bytes])
        #twat(conf, data.encode('utf_8')[0:140])
        #twat(conf, data[0:int(chunk_len)])
        
        data=data[bytes:]
        #data=data.encode('utf_8')[bytes:]
        #data=data[int(chunk_len):]
        time.sleep(timeout)
      twat(conf, 'TwatFS=end;'+meta)
    print '\nTwatting Done.'
  else:
    print '\nTwatting Canceled.'


def getfrommeta(get, meta):
  if (";%s="%get) in meta:
    got = meta[meta.find(";%s="%get)+len(get)+2:]
    return got[:got.find(';')]
  else:
    return 'unknown'
  
def recreate_file(meta, data):
  filename = getfrommeta('file', meta)
  comptype = getfrommeta('comp', meta)
  chunks = getfrommeta('chunks', meta)
  enc = getfrommeta('enc', meta)

  print "processing ", filename
  
  if len(data) != int(chunks):
    if ignore_broken_files:
      return 0
    cont=raw_input("We seem to be missing some file chunks.\nTry and write file anyway?") #this will always fail.
    if cont not in['yes','y','1']:
      return 0

  print "data reversed ", filename
  data.reverse()  # Reverse the data list, then join
  #print "Block zero: ",data[0][3]
  data = ''.join(data)

  try:
     print "trying some shit ", filename
     if enc=='utf8':
        f = open('/tmp/shit', 'wb')
        #print "about to write this: ",data
        f.write(data)
        print "wrote shit"
        f.close()

        cppHandle = subprocess.Popen('-d -i /tmp/shit -o /tmp/shot', stdin=subprocess.PIPE, shell=False,
                                executable="/home/adam/TwatFS/trunk/TwatFS/utfsmuggler",
                                stdout=subprocess.PIPE)
        print "ran the stupid decoder exe thingy "
        #decodedData = cppHandle.communicate(data)[0]
        print "got the damned data! "
        #decodedData = cppHandle.communicate(data.encode('utf_8'))[0]
        write(filename, eval('decompress_'+comptype)(decodedData))
     elif enc=='base64':
        write(filename, eval('decompress_'+comptype)(decode_base64(data)))
     else:
       print "Unknown encoding type... File save canceled."
       return
     print filename,"saved"
  except Exception,e:
    print e
    print "Could not recreate",filename

def main_down():
  name = "" #username of the twatter account storing TwatFS files
  while name == "":
    name = raw_input("Please enter the name of the twitter account to pull files from: ")
  page=1 # add support for multiple pages
  url = "https://twitter.com/statuses/user_timeline/%s.rss?count=200&page=%s" % (name,page)
  
  conf = {}
  conf['auth'] = {}
  conf['auth']['user'] = username
  conf['auth']['passwd'] = password
  conf['url']=url
  conf['dom']=1
  dom = twatty.request(conf)
  
  files = {}
  tmp=''
  
  for node in dom.getElementsByTagName("title")[1:]:
    chnk = node.firstChild.data[node.firstChild.data.find(":")+2:]
    
    if 'TwatFS=' in chnk:
      if 'TwatFS=end;' in chnk:
        print 'File Start'
        tmp=chnk
        if tmp in files.keys():
          tmp+='_' # for duplicate file names
        files[tmp]=[]
      if 'TwatFS=start;' in chnk:
        print "\nFile End\n"
        tmp=''
    elif tmp is not "":
      files[tmp].append(chnk)
      print ".",
    #elif verbose:
      #print "Twat: %s\n"%chnk

  for meta, data  in files.iteritems():
    recreate_file(meta, data)



print "========== TwatFS a05 ========="
print "======= TwatFS@gmail.com ======\n\n"
if verbose:
  print "Verbose Mode: Enabled\n"       
print 'Modes:'
print '1. Upload'
print '2. Download'
print

choice = raw_input("Choose... ")
if choice == '1':
  main_up()
if choice == '2':
  main_down()
