r=1.0;
fovc_deg=1;
f2_deg=fovc_deg/2;
f2=f2_deg*pi/180.0;
X=0.99999;
u=r/cos(f2);
m=tan(f2);
v=X*u;
#x * m+v=y
#x^2+y^2 = r^2

#(y-v)/m=x
#(y^2-2yv+v^2)/m^2=x^2
#(y^2-2yv+v^2)/m^2=r^2-y^2
#y^2(1+1/m^2)-y(2v/m^2)+(v^2/m^2-r^2)=0
m2=m*m;

a=1.0+1.0/m2;
b=-2*v/m2;
c=v*v/m2-r*r;

y=min(roots([a,b,c]))

x=sqrt(r*r-y*y);
p=[x,y]

fx=atan(X*m);
ptheta=atan2(y,x)
reflect_width_angle=ptheta+fx;
rtheta=fx+2*reflect_width_angle
