library(gridExtra)
library(lattice)
library(rgl)
library(misc3d)
tc <- read.csv("out.csv")
#p1 <- as.matrix(tc[,c("x", "y")])
#p2 <- as.matrix(tc[,c("a", "b")])
#n <- dim(p2)[1]
#tc$d <- NA
#for (i in 1:n) tc$d[i] <- sqrt(crossprod(p1[i,] - p2[i,]))
#r1<-wireframe(tc$f1 ~ tc$a + tc$b, data=tc, drape=T, scales=list(arrows=FALSE),ylab=expression(y), zlab=expression(F), xlab=expression(x))
#r2<-wireframe(tc$f1 ~ tc$x + tc$y, data=tc, scales=list(arrows=FALSE), drape=T, ylab=expression(y), zlab=expression(F), xlab=expression(x))
#grid.arrange(r1, r2, ncol=2)
#c1<-wireframe(tc$f2 ~ tc$a + tc$b, data=tc, scales=list(arrows=FALSE), drape=T, ylab=expression(y), zlab=expression(F[CP]), xlab=expression(x), screen=list(z=90,x=0,y=0))
#c2<-wireframe(tc$f2 ~ tc$x + tc$y, data=tc,  drape=T, scales=list(arrows=FALSE),ylab=expression(y), zlab=expression(F[CP]), xlab=expression(x), screen=list(z=90,x=0,y=0))
#grid.arrange(c1, c2, ncol=2)
#open3d()
plot3d(tc$f_rp, tc$a_1, tc$a_2, col=rainbow(1000))
wireframe(tc$f_rp ~ tc$a_1 + tc$a_2, data=tc, scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F[RP]), xlab=expression(x), screen=list(z=5,x=-90,y=-50))
wireframe(tc$f_rp ~ tc$a_1 + tc$a_2, data=tc, scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F[RP]), xlab=expression(x))
wireframe(tc$f_cp ~ tc$a_1 + tc$a_2, data=tc, scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F[CP]), xlab=expression(x), screen=list(z=-10,x=-90,y=-50))
#wireframe(f2 ~ x + y, drape=T, ylab=expression(y), zlab=expression(F[CP]), xlab=expression(x), screen=list(z=0,x=0,y=180), zlim=c(0,1))
#wireframe(f ~ x + y,  scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F), xlab=expression(x), screen=list(z=0,x=0,y=180), zlim=c(0,1))
#wireframe(f ~ x + y,  scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F), xlab=expression(x), screen=list(z=-10,x=-90,y=-40), zlim=c(0,1))
#plot3d(x, y, f1, col=rainbow(1000))
#wireframe(f1 ~ x + y,  scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F[RP]), xlab=expression(x), screen=list(z=-10,x=-90,y=-40))
#wireframe(f2 ~ x + y,  scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F[RP]), xlab=expression(x), screen=list(z=-10,x=-90,y=-40))
#wireframe(f ~ x + y,  scales=list(draw=F), drape=T, ylab=expression(y), zlab=expression(F[RP]), xlab=expression(x), screen=list(z=-10,x=-90,y=-40))
#p6 <- wireframe(tc$f2 ~ tc$x + tc$y, data=tc,  scales=list(draw=F), drape=T, zlab='f(x,y)', screen=list(z=-10,x=100,y=-10))
#grid.arrange(p5,p6, ncol=2)
#p7 <- wireframe(tc$f1 ~ tc$x + tc$y, data=tc,  scales=list(draw=F), drape=T, zlab='f(x,y)', screen=list(z=-10,x=120,y=-10))
#p8 <- wireframe(tc$f2 ~ tc$x + tc$y, data=tc,  scales=list(draw=F), drape=T, zlab='f(x,y)', screen=list(z=70,x=60,y=-10))
#grid.arrange(p7,p8, ncol=2)
#l <- length(tc$f1)
#pareto <- logical(length=l)
#f1.sort <- sort(tc$f1)
#f2.sort <- tc$f2[order(tc$f1)]
#x.sort <- tc$x[order(tc$f1)]
#y.sort <- tc$y[order(tc$f1)]
#a.sort <- tc$a[order(tc$f1)]
#b.sort <- tc$b[order(tc$f1)]
#for(i in 1:l) {
#    pareto[i] <- all(f2.sort[1:i] >= f2.sort[i])
#}
#plot(tc$d, tc$f2)
#plot(tc$d, tc$f1)
#plot(f1.sort[pareto], f2.sort[pareto], xlim=c(0.985, 1), ylim=c(0.0,1))
#plot(x.sort[pareto], y.sort[pareto])
#plot(a.sort[pareto], b.sort[pareto])

