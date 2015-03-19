# UTFlib a01
import base64

MAP = {}
UNMAP = {}
for C in xrange(ord('A'),ord('Z')+1):
  x = C-ord('A')
  c = chr(C)
  MAP[c] = x
  UNMAP[x] = c
for c in xrange(ord('a'),ord('z')+1):
  x = c-ord('a')+26
  c = chr(c)
  MAP[c] = x
  UNMAP[x] = c
for d in xrange(ord('0'),ord('9')+1):
  x = d-ord('0')+52
  c = chr(d)
  MAP[c] = x
  UNMAP[x] = c
MAP['+'] = 62
UNMAP[62] = '+'
MAP['/'] = 63
UNMAP[63] = '/'

# unnecessary... but just in case
MAP['='] = 64
UNMAP[64] = '='

MAPSHIFT=7

class Bitreader:
   def __init__(self):
      self.__bitindex = 0
      self.__byte = 0
      self.__type = 0 # default to data
      self.__file = None
      self.__data = None

   def get_byte(self): 
      self.__byte = self.read(1)

      if self.__byte == '':
         self.close()
         raise Exception

      self.__byte = ord(self.__byte)

   def get_bit(self):
      if self.__bitindex == 8:
         self.get_byte()
         self.__bitindex = 0

      self.__bitindex += 1
      r = self.__byte & 0x1
      self.__byte >>= 1
      return r

   def read(self, size):
      if self.__type == 1:
         return self.__file.read(1)
      else:
         if not self.__data == None and len(self.__data) == 0:
            return ''
         else:
            return self.__data.pop(0)

   def load_file(self, filename):
      self.__file = open(filename, 'rb')
      self.__type = 1
      self.get_byte()

   def load_data(self, data):
      self.__data = list(data)
      self.__type = 0
      self.get_byte()

   def close(self):
      if not self.__file is None:
         self.__file.close()

def binary_to_byte(bin):
   r = 0
   for i in xrange(8):
      if bin[i] == 1:
         r |= bin[i] << i
   return r

def encode_utf(data):   
   r = ''

   stream = Bitreader()
   stream.load_data(data)

   revrange = range(6)
   revrange.reverse()

   revrange_ao = revrange[1:]

   while 1:
      try:
         b = stream.get_bit()
         bom = [0xF0,0x80,0x80,0x80]

         bom[0] |= b << 1
         bom[0] |= stream.get_bit()

         for x in range(1,4):
            if x == 1 and bom[0] == 0xF0:
               bom[1] = 0xA0
               myrange = revrange_ao
            else:
               myrange = revrange

            for y in myrange:
               bom[x] |= stream.get_bit() << y
      except Exception, e:
         if bom[0] == 0xF0:
            bom[1] |= 0xA0
         break

      r += ''.join(map(lambda x: chr(x), bom))

   if not r[-4:] == bom:
      r += ''.join(map(lambda x: chr(x), bom))

   return r

def decode_utf(data):
   bytearray = []
   r = ''
   while len(data) > 0:
      chunk = map(lambda x: ord(x), data[:4])
      bytearray.append((chunk[0] & 0x2) >> 1)
      bytearray.append(chunk[0] & 0x1)

      for x in range(1,4):
         if x == 1 and chunk[0] == 0xF0:
            myrange = range(5)
         else:
            myrange = range(6)

         myrange.reverse()

         for y in myrange:
            bytearray.append((chunk[x] & (0x1<<y)) >> y)

      while len(bytearray) > 8:
         r += chr(binary_to_byte(bytearray[:8]))
         bytearray = bytearray[8:]

      data = data[4:]

   return r

def old_encode_utf(data):
  data = "%d;" % (len(data)) + data
  if not len(data)%9 == 0:
    data += '@' * (9-(len(data) % 9))
  b64 = base64.b64encode(data)
  r = ''
  while len(b64) > 0:
    bytemod = map(lambda x: MAP[x]+0x80, b64[:3])
    r += ''.join(map(lambda x: chr(x), [0xF1] + bytemod))
    b64 = b64[3:]
  return r

def old_decode_utf(data):
  localdata = data[:]
  b64 = ''
  while len(localdata) > 0:
    hidden_data = map(lambda x: UNMAP[ord(x) - 0x80], localdata[1:4])
    b64 += ''.join(hidden_data)
    localdata = localdata[4:]

  decoded = base64.b64decode(b64)
  (size, localdata) = decoded.split(';', 1)
  return localdata[:int(size)]
