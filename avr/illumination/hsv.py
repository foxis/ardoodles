import math

def HSVtoRGB(h, s, v):
  if s==0:
     return (v,v,v)
  else:
     var_h = h * 6/255.0;
     var_i = int( var_h );
     var_h -= var_i;
     var_1 = v * ( 1.0 - s/255.0 );
     var_2 = v * ( 1.0 - s * var_h/255.0 );
     var_3 = v * ( 1.0 - s * ( 1.0 - var_h )/255.0 );

     print h, var_i,

     if   var_i==0 or var_i==6:
       return ( v     , var_3 , var_1 )
     elif var_i==1:
       return ( var_2 , v     , var_1 )
     elif var_i==2:
       return ( var_1 , v     , var_3 )
     elif var_i==3:
       return ( var_1 , var_2 , v     )
     elif var_i==4:
       return ( var_3 , var_1 , v     )
     else:
       return ( v     , var_1 , var_2 )


a = ( [ int(j) for j in HSVtoRGB(i, 255, 255) ] for i in xrange(256) )

for i in a:
  print i

print [ int(math.pow(2.718282, math.log(255)*i/255.0)-.5) for i in xrange(256) ]
