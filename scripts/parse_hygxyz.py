#!/usr/bin/env python
from math import *
import os
import re
import sys
f = open("hygxyz.csv", "r")
f.readline()

PARSEC = 3.26163626

named_f = open("named_stars.lua", "w")
bright_f = open("bright_stars.lua", "w")
local_f = open("local_stars.lua", "w")
num = 0
tot = 0

LOCAL_RAD_SECTORS = 12.0

namesubs = {
"Gliese 729": "Ross 154",
"Gliese 447": "Ross 128",
"Gliese 905": "Ross 248",
"Phad": "Phekda",
"82 G. Eri" : "82 Gamma Eridani",
"268 G. Cet" : "268 Gamma Ceti",
"27 Tauri" : "Atlas",
"20 Tauri" : "Maia",
"23 Tauri" : "Merope",
"28 Tauri" : "Pleione",
"17 Tauri" : "Electra",
"19 Tauri" : "Taygeta",
"21 Tauri" : "Asterope",
"22 Tauri" : "Sterope"
}

gr_let = (
("Alp","Alpha"),
("Bet","Beta"),
("Gam","Gamma"),
("G.","Gamma"),
("Del","Delta"),
("Eps","Epsilon"),
("Zet","Zeta"),
#Eta
("The","Theta"),
("Iot","Iota"),
("Kap","Kappa"),
("Lam","Lambda"),
#Mu, Nu, Xi
("Omi","Omicron"),
#Pi, Rho
("Sig","Sigma"),
#Tau
("Ups","Upsilon"),
#Phi, Chi, Psi
("Ome","Omega")
)

con_names = (
("And","Andromedae"),
("Ant","Antliae"),
("Aps","Apodis"),
("Aqr","Aquarii"),
("Aql","Aquilae"),
("Ara","Arae"),
("Ari","Arietis"),
("Aur","Aurigae"),
("Boo","Bootis"),
("Cae","Caeli"),
("Cam","Camelopardalis"),
("Cnc","Cancri"),
("CVn","Canum Venaticorum"),
("CMa","Canis Majoris"),
("CMi","Canis Minoris"),
("Cap","Capricorni"),
("Car","Carinae"),
("Cas","Cassiopeiae"),
("Cen","Centauri"),
("Cep","Cephei"),
("Cet","Ceti"),
("Cha","Chamaeleontis"),
("Cir","Circini"),
("Col","Columbae"),
("Com","Comae Berenices"),
("CrA","Coronae Australis"),
("CrB","Coronae Borealis"),
("Crv","Corvi"),
("Crt","Crateris"),
("Cru","Crucis"),
("Cyg","Cygni"),
("Del","Delphini"),
("Dor","Doradus"),
("Dra","Draconis"),
("Eql","Equulei"),
("Eri","Eridani"),
("For","Fornacis"),
("Gem","Geminorum"),
("Gru","Gruis"),
("Her","Herculis"),
("Hor","Horologii"),
("Hya","Hydri"),
("Hyi","Hydri"),
("Ind","Indi"),
("Lac","Lacertae"),
("Leo","Leonis"),
("LMi","Leonis Minoris"),
("Lep","Leporis"),
("Lib","Librae"),
("Lup","Lupi"),
("Lyn","Lyncis"),
("Lyr","Lyrae"),
("Men","Mensae"),
("Mic","Microscopii"),
("Mon","Monocerotis"),
("Mus","Muscae"),
("Nor","Normae"),
("Oct","Octantis"),
("Oph","Ophiuchi"),
("Ori","Orionis"),
("Pav","Pavonis"),
("Peg","Pegasi"),
("Per","Persei"),
("Phe","Phoenicis"),
("Pic","Pictoris"),
("Psc","Piscium"),
("PsA","Piscis Austrini"),
("Pup","Puppis"),
("Pyx","Pyxidis"),
("Ret","Reticulii"),
("Sge","Sagittarii"),
("Sgr","Sagittarii"),
("Sco","Scorpii"),
("Scl","Sculptoris"),
("Sct","Scuti"),
("Ser","Serpentis"),
("Sex","Sextantis"),
("Tau","Tauri"),
("Tel","Telescopii"),
("Tri","Trianguli"),
("TrA","Trianguli Australis"),
("Tuc","Tucanae"),
("UMa","Ursae Majoris"),
("UMi","Ursae Minoris"),
("Vel","Velorum"),
("Vir","Virginis"),
("Vol","Volantis"),
("Vul","Vulpeculae"))

def query_yes_no_quit(question, default="yes"):
    """Ask a yes/no/quit question via raw_input() and return their answer.
    
    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no", "quit" or None (meaning
        an answer is required of the user).

    The "answer" return value is one of "yes", "no" or "quit".
    """
    valid = {"yes":"yes",   "y":"yes",    "ye":"yes",
             "no":"no",     "n":"no"}
    if default == None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while 1:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return default
        elif choice in valid.keys():
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes'or 'no'.\n")

class Star:
	def __init__(self, name, islocal, isnamed, sx, sy, sz, fracx, fracy, fracz):
		self.name = name
		self.islocal = islocal
		self.isnamed = isnamed
		self.sx = sx
		self.sy = sy
		self.sz = sz
		self.fracx = fracx
		self.fracy = fracy
		self.fracz = fracz
		self.components = [None, None, None, None]
	def AddComponent(self, num, component):
		self.components[num] = component

stars = {}

while 1:
	l = f.readline()
	if not l: break
	m = l.split(",")
	tot = tot + 1
	spectral = m[15]
	distance = float(m[9]) * PARSEC
	yg = float(m[17]) * PARSEC
	yg = yg + (float(m[20]) * PARSEC * 1188) #1188 years between now and Pioneer, although the data itself is likely older
	#x and y swapped to project stars in the correct orientation in relation to galactic center
	xg = float(m[18]) * PARSEC
	xg = xg + (float(m[21]) * PARSEC * 1188)
	zg = float(m[19]) * PARSEC
	zg = zg + (float(m[22]) * PARSEC * 1188)
	dist = sqrt(xg*xg + yg*yg + zg*zg)
	sx = int(floor(xg / 8.0))
	sy = int(floor(yg / 8.0))
	sz = int(floor(zg / 8.0))
	fracx = (xg/8.0) - sx
	fracy = (yg/8.0) - sy
	fracz = (zg/8.0) - sz

	name = m[6]
	magnitude = float(m[13])
	if sx > LOCAL_RAD_SECTORS-1 or sx < -LOCAL_RAD_SECTORS or \
	   sy > LOCAL_RAD_SECTORS-1 or sy < -LOCAL_RAD_SECTORS or \
	   sz > LOCAL_RAD_SECTORS-1 or sz < -LOCAL_RAD_SECTORS:
		if name:
			isNamed = 1
			isLocal = 0
		#elif magnitude > 6.0: continue
		else: 
			isLocal = 0
			isNamed = 0
	else:
		if name:
			isNamed = 1
			isLocal = 1
		else :
			isLocal = 1
			isNamed = 0
	
	# name = m[6]
	if name == "Sol": continue
	if not name and m[5]:
		# Bayer-flamsteed designation
		s = re.search("([\d]*)([A-Za-z]*)([\d\s]+)([A-Za-z]+)", m[5])
		isNamed = 1
		if s:
			flamsteed = s.group(1)
			bayer = s.group(2)
			bayer_num = s.group(3).strip()
			constellation = s.group(4)
			# Replaces UMa with Ursae Majoris, etc
			for i in con_names:
				constellation = constellation.replace(i[0], i[1])
			# Replaces Alp with Alpha
			for i in gr_let:
				bayer = bayer.replace(i[0], i[1])
			# prefer bayer, but use flamsteed if no bayer
			bits = []
			# best is bayer with no additional number
			if bayer and not bayer_num:
				name = bayer + " " + constellation
			# then flamsteed
			elif flamsteed:
				name = flamsteed + " " + constellation
			# then bayer with additional number
			elif bayer and bayer_num:
				name = bayer + "-" + bayer_num + " " + constellation
			else:
				print "WTF bayer-flamsteed: ", l
				sys.exit(0)
		else:
			print "WTFFF bayer-flamsteed: ", l
			sys.exit(0)

			
	#	bits = m[5].split()
	#	if len(bits) > 1:
	#		# Replaces UMa with Ursae Majoris, etc
	#		for i in con_names:
	#			bits[1] = bits[1].replace(i[0], i[1])
	#		# Replaces Alp with Alpha, etc
	#		for i in gr_let:
	#			bits[0] = bits[0].replace(i[0], i[1])
	#		# 
	#		wank = re.search("(\d+)+([A-Z][a-z]+)", bits[0])
	#		if wank:
	#			bits[0] = wank.group(1) + " " + wank.group(2)
	#	name = " ".join(bits)
	if not name and m[4]:
		if distance < 150.0 or magnitude < 6.0: 
			name = m[4].replace("Gl ", "Gliese ")
			isNamed = 0
		else:
			continue
	if not name and m[2]:
		isNamed = 0
		if distance < 150.0 or magnitude < 6.0: 
			name = "HD " + m[2]
		else:
			continue
	if not name:
		#if isLocal: print "WTF?: " + l
		continue
	# kill duplicate spaces
	name = " ".join(name.split())
	if name in namesubs.keys():
		name = namesubs[name]
	
	# BUT THESE MUST BE DEALT WITH!!!???
	if not spectral:
		if isLocal:
			# WHAT ARE YOU DOING!!!
			spectral = 'M'
			#print "Assuming type M: " + l
		else:
			continue

	wank = re.search("(D[ABOQZCX])(\d?)", spectral)

	if spectral == "DG":
		# For some reason Van Maanen's Star (and it alone) are
		# classified as 'DG' in this star catalogue. I can't find any
		# reference to DG spectral type for white dwarfs...
		if distance >= 50000.0:
			print "DISTANCE CHECK : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
			question = "Are you sure you want this? Its likely a messier object and in-fact not a member of our galaxy :)"  
			choice = query_yes_no_quit(question, default="no")  
			if choice == 'yes':    
				print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
				body = 'WHITE_DWARF'
			else:     
				print ("Not saved")
				pass  
		else:
			body = 'WHITE_DWARF'
			print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
	elif wank:
		if distance >= 50000.0:
			print "DISTANCE CHECK : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
			question = "Are you sure you want this? Its likely a messier object and in-fact not a member of our galaxy :)"  
			choice = query_yes_no_quit(question, default="no")  
			if choice == 'yes':    
				print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
				body = 'WHITE_DWARF'
			else:     
				print ("Not saved")
				pass  
		else:
			body = 'WHITE_DWARF'
			print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
	else:
		wank = re.search("s?d?([OBAFGKMobafgkm])(\d?)([IVivmab]*)", spectral)
		wank2 = re.search("s?d?([OBAFGKMobafgkm])(\d?)([ /MNSP:.;mnsp]*)([IVivab]*)", spectral)
		if wank and wank.group(3):
			body = 'STAR_' + wank.group(1).upper()
			size = wank.group(3).upper()
			if size[:1] == 'V':
				# dwarfs
				if distance >= 50000.0:
					print "DISTANCE CHECK : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
					question = "Are you sure you want this? Its likely a messier object and in-fact not a member of our galaxy :)"  
					choice = query_yes_no_quit(question, default="no")  
					if choice == 'yes':    
						print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
					else:     
						print ("Not saved")
						pass  
				else:
					print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
			elif size[:2] == 'IV' or size[:3] == 'III':
				# subgiants and giants
				if distance >= 50000.0:
					print "DISTANCE CHECK : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
					question = "Are you sure you want this? Its likely a messier object and in-fact not a member of our galaxy :)"  
					choice = query_yes_no_quit(question, default="no")  
					if choice == 'yes':    
						print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
						body = body + "_GIANT"
					else:     
						print ("Not saved")
						pass  
				else:
					print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
					body = body + "_GIANT"
			elif size[:2] == 'II' or size[:1] == 'I':
				# bright giants and supergiants
				if distance >= 50000.0:
					print "DISTANCE CHECK : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
					question = "Are you sure you want this? Its likely a messier object and in-fact not a member of our galaxy :)"  
					choice = query_yes_no_quit(question, default="no")  
					if choice == 'yes':    
						print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
						body = body + "_SUPER_GIANT"
					else:     
						print ("Not saved")
						pass  
				else:
					body = body + "_SUPER_GIANT"
					print "*SAVED* %s (%s) {%s} [%f,%f,%f]" % (name, spectral, body, xg, yg, zg)
			#else:
				#print "Unknown size. Not processing %s (%s) [%f,%f,%f]" % (name, spectral, xg, yg, zg)
		elif wank and wank.group(2):
			body = 'STAR_' + wank.group(1).upper()
			size = wank.group(2)
			if not wank.group(3) and not wank2.group(3):
				if size[:1] == '0':
					if distance >= 2000.0: #if below 2000LY don't bother to make this star a hypergiant, its too close 
						if distance >= 6000.0:
							if distance >= 50000.0:
								print "DISTANCE CHECK : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
								question = "Are you sure you want this? Its likely a messier object and in-fact not a member of our galaxy :)"  
								choice = query_yes_no_quit(question, default="no")  
								if choice == 'yes':    
									print ("*SAVED* as hypergiant") 
									body = body + "_HYPER_GIANT" 
								else:     
									print ("Not saved")  
							else:
								body = body + "_HYPER_GIANT" 
								print "*HYPERGIANT_SAVED* : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
						else:
							print "HYPERGIANT CHECK : %s (%s) [%f,%f,%f] \n  Magnitude : %.2f  Distance : %.2fLYs " % (name, spectral, xg, yg, zg, magnitude, distance)
							question = "Is this really a hypergiant? Check the distance and magnitude"  
							choice = query_yes_no_quit(question, default="no")  
							if choice == 'yes':    
								print ("*SAVED* as hypergiant") 
								body = body + "_HYPER_GIANT" 
							else:     
								print ("Not a Hypergiant")  
					else:
						print ("Far too close to be a hypergiant. ERROR WITH ORIGINAL DATA!! Not saving as hypergiant") 
				#else:
					#print "Not a hypergiant, skipping %s (%s) [%f,%f,%f]" % (name, spectral, xg, yg, zg)
		elif wank:
			body = 'STAR_' + wank.group(1).upper()
		else:			
			print "Unknown spectral type. Not processing %s (%s) [%f,%f,%f]" % (name, spectral, xg, yg, zg)
			continue

	component_num = 0
	if name[-1] == 'A' or name[-1] == 'B' or name[-1] == 'C' or name[-1] == 'D':
		component_num = ord(name[-1]) - ord('A')
		name = name[:-1].strip()

	if stars.has_key(name):
		stars[name].AddComponent(component_num, body)
	else:
		star = Star(name, isLocal, isNamed, sx, sy, sz, fracx, fracy, fracz)
		star.AddComponent(component_num, body)
		stars[name] = star
	num = num + 1

for i in stars.keys():
	star = stars[i]
	if star.isnamed:
		_f = named_f
	elif star.islocal:
		_f = local_f
	else:
		_f = bright_f
	components = []
	for j in star.components:
		if j: components.append(j)

	components = ",".join(["\'"+j+"\'" for j in components])
	_f.write("CustomSystem:new('%s',{%s}):add_to_sector(%d,%d,%d,v(%.3f,%.3f,%.3f))\n"%(star.name.replace('\'','\\\''),components,star.sx,star.sy,star.sz,star.fracx,star.fracy,star.fracz))
		
named_f.close()
bright_f.close()
local_f.close()
f.close()
print "Output: %d / %d stars processed" % (num, tot)
raw_input("Press ENTER to exit")
