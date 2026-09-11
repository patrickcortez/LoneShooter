import os, math, wave, struct
os.makedirs('bin/assets/sound-effects', exist_ok=True)
def w(n, f, d):
  fobj = wave.open(f'bin/assets/sound-effects/{n}.wav', 'w')
  fobj.setnchannels(1)
  fobj.setsampwidth(2)
  fobj.setframerate(44100)
  data=b''.join(struct.pack('<h', int(32767*math.cos(f*math.pi*i/44100))) for i in range(int(44100*d)))
  fobj.writeframes(data)
  fobj.close()
w('enemyfire', 300, 0.3)
w('officer-whistle', 1200, 0.5)
w('officer-command', 200, 0.4)
w('officer-retreat', 800, 0.6)
