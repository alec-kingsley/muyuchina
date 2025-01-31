function capitalize(sentence) {
    return sentence.charAt(0).toUpperCase() + sentence.slice(1);
}

function getSO(suffix) {
	// ñuqa, qam, pay, ñuqayku, ñuqanchik, qamkuna, paykuna
	paradigm = [
		["--", "yki", "ni", "--", "--", "ykichik", "ni"],
		["wanki", "--", "nki", "wayku", "--", "--", "nki"],
		["wan","sunki","n","wayku","wanchik","sunkichik","n"],
		["--","yku","yku","--","--","yku","yku"],
		["--","--","nchik","--","--","--","nchik"],
		["wankichik","--","nkichik","wayku","--","--","nkichik"],
		["wanku","sunku","nku","wayku","wanchik","sunkichik","nku"]
	];
	
	subs = ["I", "you", "he/she", "we (but not you)", "we (including you)", "you all", "they"];
	objs = ["me", "you", "him/her", "us (but not you)", "us (including you)", "you all", "them"];
	
	for (let sub = 0; sub < 7; sub++) {
		for (let obj = 0; obj < 7; obj++) {
			if (paradigm[sub][obj] == suffix) {
				return [subs[sub], objs[obj]];
			}
		}
	}
	return null;
	
}

function conjugate(verb, sub, progressive, movement, adverb) {
	var toBe = (sub=="I"?"am ":sub=="he/she"?"is ":"are ") + adverb;
	var toGo = sub=="he/she"?"goes ":"go ";
	var poss = sub=="I"?"my "
		:sub.indexOf("you")!=-1?"your "
		:sub.indexOf("we")!=-1?"our "
		:"their ";
	

	if (movement) {
			if (progressive) {
				return toBe + "on " + poss + "way to " + verb;
			} else {
				return adverb + toGo + "to " + verb;
			}
	} else {
		switch (verb) {
			case "watch":
				return progressive?(toBe + " watching")
					:(adverb+(sub=="he/she"?"watches":"watch"));
			case "know":
				return (adverb+(sub=="he/she"?"knows":"know"));
			case "carry":
				return progressive?(toBe + " carrying")
					:(adverb+(sub=="he/she"?"carries":"carry"));
			case "scare":
				return progressive?(toBe + " scaring")
					:(adverb+(sub=="he/she"?"scares":"scare"));
			case "cure":
				return progressive?(toBe + " curing")
					:(adverb+(sub=="he/she"?"cures":"cure"));
			case "remember":
				return progressive?(toBe + " remembering")
					:(adverb+(sub=="he/she"?"remembers":"remember"));
		}
	}
	return null;
}

function updateResponse() {
	var rootDict = new Map();
	rootDict.set("qhawa","watch");
	rootDict.set("riqsi","know"); // for people
	rootDict.set("q'ipi","carry");
	rootDict.set("mancha","scare");
	rootDict.set("jampi","cure");
	rootDict.set("yuyari","remember");
	
	// Get the values of each ring
	var ringVals = [
		document.getElementById("ring0").value,
		document.getElementById("ring1").value,
		document.getElementById("ring2").value,
		document.getElementById("ring3").value, // person
		document.getElementById("ring4").value,
		document.getElementById("ring5").value
	];
  
	// construct the word, ignoring the - character
	var word = "";
	for (let i = 0; i < ringVals.length; i++) {
		word += ringVals[i];
	}
	word = word.replaceAll("-","");
	document.getElementById("word").innerHTML = word;
	
	// get the subject/object based on the person suffix
	[sub, obj] = getSO(ringVals[3]);
	var hasKu = ringVals[1].indexOf("ka")!=-1 || ringVals[1]=="ku";
	var hasMu = ringVals[1].indexOf("mu")!=-1 || ringVals[1].indexOf("m-")!=-1;
	var hasPu = ringVals[1].indexOf("pu")!=-1;
	var hasLlaRaq = ringVals[2].indexOf("lla")!=-1 && ringVals[4]=="raq";
	
	// determine some invalid words
	
	if (ringVals[0] == "riqsi") {
		if (ringVals[2]=="chka" || hasPu || hasMu || ringVals[4] == "raq") {
			document.getElementById("trans").innerHTML = "Invalid word";
			return;
		}
	}
	
	if (ringVals[0] == "yuyari") {
		if (ringVals[2]=="chka" || hasMu || ringVals[4] == "raq") {
			document.getElementById("trans").innerHTML = "Invalid word";
			return;
		}
	}
	
	if (ringVals[0] == "q'ipi") {
		if (hasKu && !hasPu) {
			document.getElementById("trans").innerHTML = "Invalid word";
			return;
		}
	}
	
	if (hasKu) {
		if (obj == "him/her") {
			obj = (sub=="I"?"myself"
				:sub=="you"?"yourself"
				:sub=="he/she"?"themself"
				:sub.indexOf("we")!=-1?"ourselves"
				:sub=="you all"?"yourselves"
				:sub=="they"?"themselves"
				:sub);
		} else {
			document.getElementById("trans").innerHTML = "Invalid word";
			return;
		}
	}
	
	// build translation
	var trans = ringVals[5]=="--"?"" 
		:ringVals[5]=="puni"?"of course " 
		:ringVals[5]=="chus"?"most likely " 
		:ringVals[5]=="chá"?"maybe ":"";
	trans += ringVals[5]!="--"&&(ringVals[4]=="rayku"||ringVals[4]=="kama")?"it's ":"";

	trans += ringVals[4]=="--"?"" 
		:ringVals[4]=="rayku"?"because " 
		:ringVals[4]=="kama"?"until ":"";
	trans += sub + " ";
	trans += conjugate(
			rootDict.get(ringVals[0]), // english verb
			sub, // subject
			ringVals[2].indexOf("chka")!=-1, // progressive
			hasMu, // movement
			hasLlaRaq?"still ":ringVals[2].indexOf("lla")!=-1?"only ":"" // adverb
		) + " ";
	trans += hasPu?"it for ":"";
	trans += obj;
	trans += ringVals[4]=="raq"&&!hasLlaRaq?" first":"";
	
	document.getElementById("trans").innerHTML = capitalize(trans);
	
}

function selectRandom() {
	// Get the values of each ring
	var rings = [
		document.getElementById("ring0"),
		document.getElementById("ring1"),
		document.getElementById("ring2"),
		document.getElementById("ring3"), // person
		document.getElementById("ring4"),
		document.getElementById("ring5")
	];
	
	for (let i = 0; i < rings.length; i++) {
		var ringOpts = rings[i].getElementsByTagName("option");
		rings[i].value = ringOpts[Math.floor(ringOpts.length*Math.random())].value;
	}
	
	updateResponse();
	
}