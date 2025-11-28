import struct
import math
import os

def create_bmp(filename, width, height, pattern_type="checker"):
    # BMP Header
    file_size = 54 + width * height * 3
    # Format: 2s (BM) I (size) H (res) H (res) I (offset) I (header size) I (w) I (h) H (planes) H (bpp) I (compression) I (img size) I (xres) I (yres) I (colors) I (important)
    # Total items: 16
    # 2s + I + H + H + I + I + I + I + H + H + I + I + I + I + I + I
    # <2sIHHIIIIHHIIIIII
    header = struct.pack('<2sIHHIIIIHHIIIIII', b'BM', file_size, 0, 0, 54, 40, width, height, 1, 24, 0, width * height * 3, 0, 0, 0, 0)
    
    with open(filename, 'wb') as f:
        f.write(header)
        for y in range(height):
            for x in range(width):
                r, g, b = 0, 0, 0
                if pattern_type == "checker":
                    if ((x // 32) + (y // 32)) % 2 == 0:
                        r, g, b = 200, 200, 200
                    else:
                        r, g, b = 100, 100, 100
                elif pattern_type == "noise":
                    import random
                    val = random.randint(100, 200)
                    r, g, b = val, val, val
                elif pattern_type == "bricks":
                    if y % 32 < 2 or x % 64 < 2:
                        r, g, b = 50, 50, 50 # Mortar
                    else:
                        r, g, b = 150, 75, 50 # Brick
                
                f.write(struct.pack('BBB', b, g, r)) # BGR format

def create_obj_cube(filename):
    with open(filename, 'w') as f:
        f.write("# Cube\n")
        vertices = [
            (-1, -1, -1), (1, -1, -1), (1, 1, -1), (-1, 1, -1),
            (-1, -1, 1), (1, -1, 1), (1, 1, 1), (-1, 1, 1)
        ]
        for v in vertices:
            f.write(f"v {v[0]} {v[1]} {v[2]}\n")
            
        f.write("vt 0 0\nvt 1 0\nvt 1 1\nvt 0 1\n")
        f.write("vn 0 0 -1\nvn 0 0 1\nvn 0 -1 0\nvn 0 1 0\nvn -1 0 0\nvn 1 0 0\n")
        
        # Faces (v/vt/vn)
        f.write("f 1/1/1 2/2/1 3/3/1 4/4/1\n") # Back
        f.write("f 5/1/2 8/4/2 7/3/2 6/2/2\n") # Front
        f.write("f 1/1/3 5/2/3 6/3/3 2/4/3\n") # Bottom
        f.write("f 4/1/4 3/2/4 7/3/4 8/4/4\n") # Top
        f.write("f 1/1/5 4/2/5 8/3/5 5/4/5\n") # Left
        f.write("f 2/1/6 6/2/6 7/3/6 3/4/6\n") # Right

def create_obj_tree(filename):
    with open(filename, 'w') as f:
        f.write("# Tree\n")
        # Trunk
        f.write("v -0.5 0 -0.5\nv 0.5 0 -0.5\nv 0.5 2 -0.5\nv -0.5 2 -0.5\n")
        f.write("v -0.5 0 0.5\nv 0.5 0 0.5\nv 0.5 2 0.5\nv -0.5 2 0.5\n")
        # Leaves (Pyramid)
        f.write("v -2 2 -2\nv 2 2 -2\nv 2 2 2\nv -2 2 2\nv 0 5 0\n")
        
        f.write("f 1 2 3 4\nf 5 8 7 6\nf 1 5 6 2\nf 2 6 7 3\nf 3 7 8 4\nf 4 8 5 1\n") # Trunk faces
        f.write("f 9 10 13\nf 10 11 13\nf 11 12 13\nf 12 9 13\nf 9 12 11 10\n") # Leaves faces

def create_obj_humanoid(filename):
    with open(filename, 'w') as f:
        f.write("# Low-poly Adventurer\n")
        
        # Helper to write a box
        def write_box(x, y, z, w, h, d, v_start):
            hw, hh, hd = w/2, h, d/2
            verts = [
                (x-hw, y, z-hd), (x+hw, y, z-hd), (x+hw, y+hh, z-hd), (x-hw, y+hh, z-hd),
                (x-hw, y, z+hd), (x+hw, y, z+hd), (x+hw, y+hh, z+hd), (x-hw, y+hh, z+hd)
            ]
            for v in verts:
                f.write(f"v {v[0]} {v[1]} {v[2]}\n")
            
            indices = [
                (1,2,3,4), (5,8,7,6), (1,5,6,2), (4,3,7,8), (1,4,8,5), (2,6,7,3)
            ]
            for face in indices:
                v1, v2, v3, v4 = [i + v_start - 1 for i in face]
                f.write(f"f {v1} {v2} {v3} {v4}\n")
            return v_start + 8

        v_idx = 1
        # Boots
        v_idx = write_box(-0.25, 0, 0, 0.2, 0.3, 0.3, v_idx)
        v_idx = write_box(0.25, 0, 0, 0.2, 0.3, 0.3, v_idx)
        # Legs
        v_idx = write_box(-0.25, 0.3, 0, 0.18, 0.7, 0.18, v_idx)
        v_idx = write_box(0.25, 0.3, 0, 0.18, 0.7, 0.18, v_idx)
        # Belt
        v_idx = write_box(0, 1.0, 0, 0.6, 0.1, 0.3, v_idx)
        # Torso
        v_idx = write_box(0, 1.1, 0, 0.55, 0.7, 0.25, v_idx)
        # Shoulders
        v_idx = write_box(-0.35, 1.6, 0, 0.2, 0.2, 0.2, v_idx)
        v_idx = write_box(0.35, 1.6, 0, 0.2, 0.2, 0.2, v_idx)
        # Arms
        v_idx = write_box(-0.35, 1.1, 0, 0.15, 0.5, 0.15, v_idx)
        v_idx = write_box(0.35, 1.1, 0, 0.15, 0.5, 0.15, v_idx)
        # Hands
        v_idx = write_box(-0.35, 0.9, 0, 0.15, 0.2, 0.15, v_idx)
        v_idx = write_box(0.35, 0.9, 0, 0.15, 0.2, 0.15, v_idx)
        # Neck
        v_idx = write_box(0, 1.8, 0, 0.15, 0.1, 0.15, v_idx)
        # Head
        v_idx = write_box(0, 1.9, 0, 0.3, 0.35, 0.3, v_idx)
        # Hat (Brim)
        v_idx = write_box(0, 2.15, 0, 0.6, 0.05, 0.6, v_idx)
        # Hat (Top)
        v_idx = write_box(0, 2.2, 0, 0.3, 0.2, 0.3, v_idx)
        # Backpack
        v_idx = write_box(0, 1.2, -0.2, 0.4, 0.5, 0.2, v_idx)

def create_obj_ground(filename):
    with open(filename, 'w') as f:
        f.write("# Tiled Ground Plane\n")
        size = 50.0
        tiles = 10
        step = size * 2 / tiles
        
        # Vertices
        for z in range(tiles + 1):
            for x in range(tiles + 1):
                px = -size + x * step
                pz = -size + z * step
                f.write(f"v {px} 0 {pz}\n")
                # Texture coords (repeat 5 times across)
                u = x / tiles * 5
                v = z / tiles * 5
                f.write(f"vt {u} {v}\n")
        
        f.write("vn 0 1 0\n") # Up normal
        
        # Faces
        for z in range(tiles):
            for x in range(tiles):
                # Indices (1-based)
                # Top-Left: z * (tiles+1) + x + 1
                # Top-Right: z * (tiles+1) + (x+1) + 1
                # Bot-Left: (z+1) * (tiles+1) + x + 1
                # Bot-Right: (z+1) * (tiles+1) + (x+1) + 1
                
                tl = z * (tiles + 1) + x + 1
                tr = tl + 1
                bl = (z + 1) * (tiles + 1) + x + 1
                br = bl + 1
                
                # Use same index for v and vt, normal is 1
                f.write(f"f {tl}/{tl}/1 {bl}/{bl}/1 {br}/{br}/1 {tr}/{tr}/1\n")

import math

def create_wav(filename, freq=440.0, duration=0.5):
    sample_rate = 44100
    num_samples = int(sample_rate * duration)
    
    # WAV Header
    header = bytearray()
    header += b'RIFF'
    header += struct.pack('<I', 36 + num_samples * 2)
    header += b'WAVEfmt '
    header += struct.pack('<I', 16)
    header += struct.pack('<H', 1)
    header += struct.pack('<H', 1)
    header += struct.pack('<I', sample_rate)
    header += struct.pack('<I', sample_rate * 2)
    header += struct.pack('<H', 2)
    header += struct.pack('<H', 16)
    header += b'data'
    header += struct.pack('<I', num_samples * 2)
    
    with open(filename, 'wb') as f:
        f.write(header)
        for i in range(num_samples):
            t = float(i) / sample_rate
            # Simple sine wave with decay
            value = 32767.0 * math.sin(2.0 * math.pi * freq * t) * (1.0 - t/duration)
            # Clamp to valid range
            value = max(-32767, min(32767, int(value)))
            f.write(struct.pack('<h', value))

def create_background_music(filename, duration=60.0):
    """Create professional-quality game background music"""
    sample_rate = 44100
    num_samples = int(sample_rate * duration)
    
    # WAV Header
    header = bytearray()
    header += b'RIFF'
    header += struct.pack('<I', 36 + num_samples * 2)
    header += b'WAVEfmt '
    header += struct.pack('<I', 16)
    header += struct.pack('<H', 1)
    header += struct.pack('<H', 1)
    header += struct.pack('<I', sample_rate)
    header += struct.pack('<I', sample_rate * 2)
    header += struct.pack('<H', 2)
    header += struct.pack('<H', 16)
    header += b'data'
    header += struct.pack('<I', num_samples * 2)
    
    # Musical notes (frequencies in Hz)
    # C major scale
    notes = {
        'C4': 261.63, 'D4': 293.66, 'E4': 329.63, 'F4': 349.23,
        'G4': 392.00, 'A4': 440.00, 'B4': 493.88, 'C5': 523.25,
        'D5': 587.33, 'E5': 659.25, 'F5': 698.46, 'G5': 783.99
    }
    
    # Melody pattern (adventure game style)
    melody = ['C5', 'E5', 'G5', 'E5', 'D5', 'F5', 'E5', 'C5',
              'G4', 'C5', 'E5', 'D5', 'C5', 'G4', 'A4', 'C5']
    
    # Bass pattern
    bass = ['C4', 'C4', 'G4', 'G4', 'F4', 'F4', 'C4', 'C4']
    
    with open(filename, 'wb') as f:
        f.write(header)
        
        beat_duration = 0.5  # Half second per beat
        samples_per_beat = int(sample_rate * beat_duration)
        
        for i in range(num_samples):
            t = float(i) / sample_rate
            
            # Determine which beat we're on
            beat = int(t / beat_duration)
            melody_note = melody[beat % len(melody)]
            bass_note = bass[(beat // 2) % len(bass)]
            
            # Time within current beat for envelope
            beat_t = (t % beat_duration) / beat_duration
            
            # Envelope (fade in/out for each note)
            envelope = math.sin(beat_t * math.pi)
            
            # Melody (higher octave, softer)
            melody_freq = notes[melody_note]
            melody_val = 8000 * math.sin(2.0 * math.pi * melody_freq * t) * envelope
            
            # Harmony (third above melody)
            harmony_val = 4000 * math.sin(2.0 * math.pi * melody_freq * 1.25 * t) * envelope
            
            # Bass (lower, sustained)
            bass_freq = notes[bass_note]
            bass_val = 6000 * math.sin(2.0 * math.pi * bass_freq * t)
            
            # Subtle pad (sustained chords)
            pad_val = 2000 * math.sin(2.0 * math.pi * melody_freq * 0.5 * t)
            
            # Mix all layers
            value = melody_val + harmony_val + bass_val + pad_val
            value = max(-32767, min(32767, int(value)))
            f.write(struct.pack('<h', value))

if __name__ == "__main__":
    if not os.path.exists("assets"):
        os.makedirs("assets")
        
    if not os.path.exists("assets/wall.bmp"):
        create_bmp("assets/wall.bmp", 256, 256, "bricks")
    if not os.path.exists("assets/ground.bmp"):
        create_bmp("assets/ground.bmp", 512, 512, "noise") # Better ground texture
    
    if not os.path.exists("assets/pillar.obj"):
        create_obj_cube("assets/pillar.obj")
    if not os.path.exists("assets/tree.obj"):
        create_obj_tree("assets/tree.obj")
    if not os.path.exists("assets/rock.obj"):
        create_obj_cube("assets/rock.obj")
    if not os.path.exists("assets/player.obj"):
        create_obj_humanoid("assets/player.obj")
    if not os.path.exists("assets/ground.obj"):
        create_obj_ground("assets/ground.obj")
    
    # Sound definitions: (filename, frequency, duration)
    sound_data = [
        ("collect.wav", 880.0, 0.3),
        ("chest.wav", 440.0, 0.5),
        ("crack.wav", 150.0, 0.2),
        ("fall.wav", 100.0, 1.0),
        ("damage.wav", 200.0, 0.4),
        ("jump.wav", 600.0, 0.2),
        ("portal.wav", 1200.0, 2.0),
        ("win.wav", 1000.0, 3.0),
        ("growl.wav", 100.0, 0.8)
    ]
    
    for s, freq, dur in sound_data:
        # Always regenerate sounds to ensure they work
        create_wav(f"assets/{s}", freq, dur)
    
    # Generate background music (longer, multi-tone)
    print("Generating background music...")
    create_background_music("assets/background.wav")
        
    print("Assets generated successfully.")

def create_background_music(filename, duration=30.0):
    """Create a simple background music loop"""
    sample_rate = 44100
    num_samples = int(sample_rate * duration)
    
    # WAV Header
    header = bytearray()
    header += b'RIFF'
    header += struct.pack('<I', 36 + num_samples * 2)
    header += b'WAVEfmt '
    header += struct.pack('<I', 16)
    header += struct.pack('<H', 1)
    header += struct.pack('<H', 1)
    header += struct.pack('<I', sample_rate)
    header += struct.pack('<I', sample_rate * 2)
    header += struct.pack('<H', 2)
    header += struct.pack('<H', 16)
    header += b'data'
    header += struct.pack('<I', num_samples * 2)
    
    with open(filename, 'wb') as f:
        f.write(header)
        # Simple chord progression
        freqs = [440.0, 554.37, 659.25, 523.25]  # A, C#, E, C
        for i in range(num_samples):
            t = float(i) / sample_rate
            # Mix multiple frequencies for richer sound
            chord_idx = int((t / duration) * len(freqs)) % len(freqs)
            freq = freqs[chord_idx]
            value = 0
            value += 16000 * math.sin(2.0 * math.pi * freq * t)
            value += 8000 * math.sin(2.0 * math.pi * freq * 2 * t)
            value += 4000 * math.sin(2.0 * math.pi * freq * 0.5 * t)
            f.write(struct.pack('<h', int(value)))
