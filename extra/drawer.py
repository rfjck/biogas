# -*- coding: utf-8 -*-
import pygame
from os import system
import sys

from math import log,ceil

"""
Este código es una verguenza, fue hecho a la rápida, pero logra su objetivo.
No tocar.
"""

def main(predfile,filenames,dimx,dimy,inside,zoom_steps=1,last_steps=0):
	minx2= 99999999999L
	minx= 99999999999L
	maxx2= 0
	maxx= 0
	miny2= 99999999999L
	miny= 99999999999L
	maxy2= 0
	maxy= 0
	fil= open(predfile)
	linn=0
	print "reading: \t"+predfile
	predios=[]
	for lin in fil:
		linn+=1
		#if linn==1: continue
		lin= lin.split(';')
		lin= (int(lin[1]),int(lin[2]))
		minx2= lin[0] if lin[0]<minx2 else minx2
		maxx2= lin[0] if lin[0]>maxx2 else maxx2
		miny2= lin[1] if lin[1]<miny2 else miny2
		maxy2= lin[1] if lin[1]>maxy2 else maxy2
		predios.append(lin)

	pasos=[]
	paso=[]
	while True:
		try:
			print "reading: \t"+filenames+"/paso_"+str(len(pasos)+1)+"_mejor_"+str(len(paso)+1)+"_predios.csv"
			fil= open(filenames+"/paso_"+str(len(pasos)+1)+"_mejor_"+str(len(paso)+1)+"_predios.csv","r")
		except:
			if len(paso)>0:
				pasos.append(paso)
				paso=[]
				continue
			else:
				break
		linn=0
		lines=[]
		for lin in fil:
			linn+=1
			if linn==1: continue
			lin= lin.split(",")
			lin= (int(lin[1]),int(lin[2]),int(lin[4]),int(lin[5]))
			minx= lin[0] if lin[0]<minx else minx
			maxx= lin[0] if lin[0]>maxx else maxx
			minx= lin[2] if lin[2]<minx else minx
			maxx= lin[2] if lin[2]>maxx else maxx
			miny= lin[1] if lin[1]<miny else miny
			maxy= lin[1] if lin[1]>maxy else maxy
			miny= lin[3] if lin[3]<miny else miny
			maxy= lin[3] if lin[3]>maxy else maxy
			lines.append(lin)
		fil.close()
		paso.append(lines)
	print "limits:\t",minx,maxx,miny,maxy
	coef= float( min(dimx/float(maxx-minx),dimy/float(maxy-miny)) )*inside
	coef2= float( min(dimx/float(maxx2-minx2),dimy/float(maxy2-miny2)) )*inside
	print "zoom:\t",coef
	horcoef= ((dimx/float(maxx-minx))*inside)/coef

	pygame.font.init()
	font= pygame.font.Font(None,24)

	import utm
	lat,lon = utm.to_latlon((minx+maxx)/2,(miny+maxy)/2, 18, northern=False)
	_,lon3 = utm.to_latlon((minx+maxx)/2-((minx+maxx)/2-minx)*horcoef/inside,(miny+maxy)/2, 18, northern=False)
	_,lon2 = utm.to_latlon((minx+maxx)/2+(maxx-(minx+maxx)/2)*horcoef/inside,(miny+maxy)/2, 18, northern=False)
	nn= 7
	print "size nn:\t",nn

	comand= "wget \"http://maps.google.com/maps/api/staticmap?center="+str(lat)+","+str(lon)+"&maptype=roadmap&zoom="+str(nn)+"&size=9999x9999&maptype=roadmap&sensor=false\" -O mmedia/fondo.png"
	print comand
	system(comand)
	fondo_primigenio1p= pygame.image.load("mmedia/fondo.png")
	fondo_primigenio1= pygame.Surface(fondo_primigenio1p.get_size(),depth=32)
	fondo_primigenio1.blit(fondo_primigenio1p,(0,0))
	zoom= (dimx*360.0)/((lon2-lon3)*512.0*2**(nn-1))

	comand= "wget \"http://maps.google.com/maps/api/staticmap?center="+str(lat)+","+str(lon)+"&maptype=roadmap&zoom="+str(nn+1)+"&size=9999x9999&maptype=roadmap&sensor=false\" -O mmedia/fondo2.png"
	print comand
	system(comand)
	fondo_primigenio2p= pygame.image.load("mmedia/fondo2.png")
	fondo_primigenio2= pygame.Surface(fondo_primigenio2p.get_size(),depth=32)
	fondo_primigenio2.blit(fondo_primigenio2p,(0,0))
	zoom2= (dimx*360.0)/((lon2-lon3)*512.0*2**(nn))




	print "Dibujando predios..."
	used_preds=set([])
	for paso in pasos:
		for group in paso:
			for line in group:
				for co in (0,1):
					lin=line[co*2:]
					used_preds.add((lin[0],lin[1]))
	#zoom_steps=60
	for i in xrange(zoom_steps+1):
		fornow_coef= (i*coef+(zoom_steps-i)*coef2)/float(zoom_steps)
		fondo= pygame.transform.smoothscale(fondo_primigenio1,(int(fondo_primigenio1.get_size()[0]*zoom*fornow_coef/coef),int(fondo_primigenio1.get_size()[1]*zoom*fornow_coef/coef) ))
		fondo2= pygame.transform.smoothscale(fondo_primigenio2,(int(fondo_primigenio2.get_size()[0]*zoom2*fornow_coef/coef),int(fondo_primigenio2.get_size()[1]*zoom2*fornow_coef/coef) ))
		base= pygame.Surface((dimx,dimy),pygame.SRCALPHA)
		#base.fill((255,255,255,0))
		surf_wall= pygame.Surface((dimx,dimy),pygame.SRCALPHA)
		surf_wall.set_alpha(255)
		surf_wall.blit(fondo,((dimx-fondo.get_size()[0])/2,(dimy-fondo.get_size()[1])/2))
		surf_wall.blit(fondo2,((dimx-fondo2.get_size()[0])/2,(dimy-fondo2.get_size()[1])/2))
		for lin in predios:
			pygame.draw.rect(base,(127,127,255),
				(dimx/2+(lin[0]-(maxx+minx)/2)*fornow_coef-2,dimy/2-(lin[1]-(maxy+miny)/2)*fornow_coef-2,5,5),1)
		for lin in used_preds:
			pygame.draw.rect(base,(0,0,255),
				(dimx/2+(lin[0]-(maxx+minx)/2)*fornow_coef-2,dimy/2-(lin[1]-(maxy+miny)/2)*fornow_coef-2,5,5),1)
		print "generating:\t"+"mmedia/"+str(i)+".png"
		surf_wall.blit(base,(0,0))
		if i<zoom_steps :
			pygame.image.save(surf_wall,"mmedia/"+str(i)+".png")

	pason=0
	for paso in pasos:
		suptext= font.render(u"Tamaños probados: "+str(pason+1),False,(0,0,0))
		surf= surf_wall.copy() #pygame.Surface((dimx,dimy),pygame.SRCALPHA)
		#surf.fill((255,255,255,255))
		groupn=len(paso)
		tot= groupn
		for group in paso[::-1]:
			for line in group:
				if groupn==1:
					color= (0,128,0)
				else:
					parcial= (groupn*512)//tot
					color= (min(parcial,255),min(255,512-parcial),0)
				pygame.draw.line(surf,color,
					(dimx/2+(line[0]-(maxx+minx)/2)*coef,dimy/2-(line[1]-(maxy+miny)/2)*coef),
					(dimx/2+(line[2]-(maxx+minx)/2)*coef,dimy/2-(line[3]-(maxy+miny)/2)*coef),1)
			groupn-=1
		surf.blit(base,(0,0))
		surf.blit(suptext,(1,1))
		for extra in xrange(0,last_steps+1 if pason==len(pasos)-1 else 1):
			print "generating:\t"+"mmedia/"+str(pason+zoom_steps+extra)+".png"
			pygame.image.save(surf,"mmedia/"+str(pason+zoom_steps+extra)+".png")
		pason+=1

main(sys.argv[1],"./build/interm",1024,778,0.96)#,60,48)
