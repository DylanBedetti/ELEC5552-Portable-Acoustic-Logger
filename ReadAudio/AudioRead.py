from scipy.io import wavfile
from scipy import signal
import numpy as np
import noisereduce as nr
import numpy as np
import wavio
# load data
wavObject = wavio.read("A2.wav").data
# perform noise reduction
data=np.squeeze(wavObject.data)
reduced_noise = nr.reduce_noise(audio_clip=data,noise_clip=data, verbose=True)

#wavfile.write('test2.wav', sr, x.astype(np.int16))