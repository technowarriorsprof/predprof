from pydub import AudioSegment

def convert_wav_to_mp3(wav_file, mp3_file, bitrate='256k'):
    audio = AudioSegment.from_wav(wav_file)
    audio.export(mp3_file, format='mp3', bitrate=bitrate)
convert_wav_to_mp3('record.wav', 'output.mp3')
