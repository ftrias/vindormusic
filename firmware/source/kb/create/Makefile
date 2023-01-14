DEBUG = -m -dm
all: __target__/convert.js 

release: chart.html kbfiles.html
	transcrypt -b -k convert.py
	cp wrap.js __target__
	cd __target__; npx webpack wrap.js 
	cat __target__/dist/main.js | pbcopy

__target__/convert.js: *.py wrap.js
	transcrypt $(DEBUG) -k convert.py

init:
	mkdir __target__
	cd __target__; npm install --save-dev webpack
	cd __target__; npm install --save-dev webpack-cli

kbfiles.html: *.py
	./analyze.py --vnd --out kbfiles.html > kbfiles.log
	# cat kbfiles.html | pbcopy

chart.html: models.py
	./analyze.py -g

clean:
	rm -rf __target__
