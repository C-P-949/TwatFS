#modified from transmision.py lib
import socket, urllib, urllib2, cgi, sys
from xml.dom import minidom

def request(obj):
  if obj.has_key('data'):
    data = urllib.urlencode(obj['data'])
  else:
    data=None

  if obj.has_key('auth'):    
    auth = urllib2.HTTPPasswordMgrWithDefaultRealm()
    auth.add_password(None, 'https://twitter.com/statuses/', obj['auth'].get('user',None), obj['auth'].get('passwd',None))
    authHandler = urllib2.HTTPBasicAuthHandler(auth)
  else: 
    authHandler = ''
    
  opener = urllib2.build_opener(authHandler)
  urllib2.install_opener(opener)  
  
  #if o.has_key('timeout'):
  #  socket.setdefaulttimeout(cur_timeout)
  
  if obj.has_key('url'):
    request = urllib2.Request(obj['url'],data)
    #request = urllib2.Request(obj['url'])
  else:
    print "No URL Specified"
  
  resp = ''
  
  try:
      if obj.has_key('dom'):
        from xml.dom import minidom
        resp = minidom.parse(urllib2.urlopen(request))
      else:
        resp = urllib2.urlopen(request).read()
 
  except IOError, e:
    if hasattr(e, 'reason'):
      print 'Nework Failed:', e.reason
      return 0
   
    elif hasattr(e, 'code'):
      print 'Request Fails, error code: ', e.code
      return 0
    
  return resp
