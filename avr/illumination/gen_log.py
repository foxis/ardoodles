"""

 a = xrange(0,256)
 x = xrange(0,256)

 x = kln(a)

 x[i+1]-x[i] = kln(a[i+1]/a[i])
 c = kln(a[i+1]/a[i])
 c/k = ln(a[i+1]/a[i])

 e**(c/k) * a[i] = a[i+1]

 a[1] = 1
 i = xrange(N)

 a[i] = e**(c * (i-1))

 c = ln(a[n-1]/a[0]) / n

"""


print [ int(1*math.e**(math.log(128.0)*i/127.0)+.5) for i in xrange(128) ]
print [ int(1*math.e**(math.log(256.0)*i/255.0)+.5) for i in xrange(256) ]

