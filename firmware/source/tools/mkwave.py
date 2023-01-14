import math

pi2 = math.radians(360)
step = pi2 / 256

def abs(x):
	if x<0: return -x
	return x
def sign(x):
	if x<0: return -1
	return 1
def form(x):
	# return math.sin(x)
	# sin(x)^0.25
	return math.pow(abs(math.sin(x)),0.5) * sign(math.sin(x))

print("""
	const int16_t sine_table[257] = {
""", end="")

err = 0

for i in range(0, 257):
	a = form(i * step) * 32767 + err
	n = round(a)
	err = n - a
	if i == 0: print("            ", end="")
	print("%6d" % n, end="")
	if (i+1) % 10 == 0: print(",\n            ", end="")
	elif i<256: print(",", end="")
print("""
	};

	const float sine_table_float[257] = {
""")

for i in range(0, 257):
	a = form(i * step)
	if i == 0: print("            ", end="")
	print("%6f" % a, end="")
	if (i+1) % 5 == 0: print(",\n            ", end="")
	elif i<256: print(",", end="")
print("""
	};
""")