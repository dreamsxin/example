import math

def ComputeEuclideanDistance(x1, y1, x2, y2):
    d = math.sqrt(math.pow((x1-x2), 2) + math.pow((y1 - y2), 2))
    return d

d_ag = ComputeEuclideanDistance(3, 104, 18, 90)
d_bg = ComputeEuclideanDistance(2, 100, 18, 90)
d_cg = ComputeEuclideanDistance(1, 81, 18, 90)
d_dg = ComputeEuclideanDistance(101, 10, 18, 90)
d_eg = ComputeEuclideanDistance(99, 5, 18, 90)
d_fg = ComputeEuclideanDistance(98, 2, 18, 90)
print "d_ag:", d_ag
print "d_bg:", d_bg
print "d_cg:", d_cg
print "d_dg:", d_dg
print "d_eg:", d_eg
print "d_fg:", d_fg