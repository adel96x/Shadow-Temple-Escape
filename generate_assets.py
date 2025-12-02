#!/usr/bin/env python3
"""
Enhanced Asset Generator for Shadow Temple Escape
Creates professional-quality textures and improved 3D models
"""

import struct
import math
import os
import random

# =============================================================================
# PROFESSIONAL TEXTURE GENERATION
# =============================================================================

def create_sand_texture(filename, width=512, height=512):
    """Create realistic sand texture with grain and variation"""
    print(f"Generating professional sand texture: {filename}")
    
    file_size = 54 + width * height * 3
    header = struct.pack('<2sIHHIIIIHHIIIIII', 
                        b'BM', file_size, 0, 0, 54, 40, 
                        width, height, 1, 24, 0, 
                        width * height * 3, 0, 0, 0, 0)
    
    with open(filename, 'wb') as f:
        f.write(header)
        
        # Base sand colors (warm desert sand)
        base_colors = [
            (237, 201, 175),  # Light sand
            (220, 182, 150),  # Medium sand
            (210, 175, 140),  # Darker sand
            (200, 165, 130),  # Deep sand
        ]
        
        for y in range(height):
            for x in range(width):
                # Multi-octave noise for realistic sand grain
                noise = 0
                frequency = 1
                amplitude = 1
                for octave in range(4):
                    nx = x * frequency / width
                    ny = y * frequency / height
                    noise += (math.sin(nx * 10 + ny * 7) * 0.5 + 0.5) * amplitude
                    frequency *= 2
                    amplitude *= 0.5
                
                noise = noise / 2.0  # Normalize
                
                # Select base color with variation
                color_index = int(noise * (len(base_colors) - 1))
                color_index = max(0, min(len(base_colors) - 1, color_index))
                base_r, base_g, base_b = base_colors[color_index]
                
                # Add fine grain detail
                grain = random.randint(-15, 15)
                
                # Add slight color variation for realism
                r = max(0, min(255, base_r + grain + random.randint(-5, 5)))
                g = max(0, min(255, base_g + grain + random.randint(-5, 5)))
                b = max(0, min(255, base_b + grain + random.randint(-5, 5)))
                
                f.write(struct.pack('BBB', b, g, r))

def create_snow_texture(filename, width=512, height=512):
    """Create realistic snow texture with sparkle effect"""
    print(f"Generating professional snow texture: {filename}")
    
    file_size = 54 + width * height * 3
    header = struct.pack('<2sIHHIIIIHHIIIIII', 
                        b'BM', file_size, 0, 0, 54, 40, 
                        width, height, 1, 24, 0, 
                        width * height * 3, 0, 0, 0, 0)
    
    with open(filename, 'wb') as f:
        f.write(header)
        
        for y in range(height):
            for x in range(width):
                # Base snow is very bright white with slight blue tint
                base_r, base_g, base_b = 245, 248, 255
                
                # Add subtle texture variation
                noise = (math.sin(x * 0.1) + math.cos(y * 0.1)) * 5
                
                # Random sparkle effect (ice crystals)
                if random.random() > 0.98:
                    sparkle = random.randint(20, 40)
                else:
                    sparkle = 0
                
                # Slight shadow variation for depth
                shadow = int((math.sin(x * 0.05 + y * 0.05)) * 8)
                
                r = max(200, min(255, int(base_r + noise + sparkle + shadow)))
                g = max(205, min(255, int(base_g + noise + sparkle + shadow)))
                b = max(210, min(255, int(base_b + noise + sparkle + shadow)))
                
                f.write(struct.pack('BBB', b, g, r))

def create_ice_wall_texture(filename, width=256, height=256):
    """Create icy blue wall texture"""
    print(f"Generating ice wall texture: {filename}")
    
    file_size = 54 + width * height * 3
    header = struct.pack('<2sIHHIIIIHHIIIIII', 
                        b'BM', file_size, 0, 0, 54, 40, 
                        width, height, 1, 24, 0, 
                        width * height * 3, 0, 0, 0, 0)
    
    with open(filename, 'wb') as f:
        f.write(header)
        
        for y in range(height):
            for x in range(width):
                # Icy blue-white gradient
                base_r = 180 + int(math.sin(x * 0.1) * 20)
                base_g = 210 + int(math.cos(y * 0.1) * 20)
                base_b = 240 + int(math.sin((x + y) * 0.05) * 15)
                
                # Ice crack pattern
                if (x % 32 < 2 or y % 32 < 2) and random.random() > 0.7:
                    base_r -= 40
                    base_g -= 30
                    base_b -= 20
                
                r = max(0, min(255, base_r))
                g = max(0, min(255, base_g))
                b = max(0, min(255, base_b))
                
                f.write(struct.pack('BBB', b, g, r))

def create_sandstone_wall_texture(filename, width=256, height=256):
    """Create sandstone brick wall texture"""
    print(f"Generating sandstone wall texture: {filename}")
    
    file_size = 54 + width * height * 3
    header = struct.pack('<2sIHHIIIIHHIIIIII', 
                        b'BM', file_size, 0, 0, 54, 40, 
                        width, height, 1, 24, 0, 
                        width * height * 3, 0, 0, 0, 0)
    
    with open(filename, 'wb') as f:
        f.write(header)
        
        for y in range(height):
            for x in range(width):
                # Sandstone base colors
                base_r, base_g, base_b = 210, 180, 140
                
                # Brick pattern with offset rows
                brick_h = 32
                brick_w = 64
                row = y // brick_h
                offset = (row % 2) * (brick_w // 2)
                
                # Mortar lines
                if (y % brick_h < 2) or ((x + offset) % brick_w < 2):
                    # Darker mortar
                    base_r, base_g, base_b = 160, 140, 100
                else:
                    # Add texture to bricks
                    texture = random.randint(-10, 10)
                    base_r += texture
                    base_g += texture
                    base_b += int(texture * 0.8)
                
                r = max(0, min(255, base_r))
                g = max(0, min(255, base_g))
                b = max(0, min(255, base_b))
                
                f.write(struct.pack('BBB', b, g, r))

# =============================================================================
# ENHANCED 3D MODELS
# =============================================================================

def create_detailed_player(filename):
    """Create a detailed adventurer character"""
    print(f"Generating detailed player model: {filename}")
    
    with open(filename, 'w') as f:
        f.write("# Detailed Adventure Character\n")
        f.write("# Created for Shadow Temple Escape\n\n")
        
        def write_box(x, y, z, w, h, d, v_start):
            """Helper to write a box with proper normals"""
            hw, hh, hd = w/2, h/2, d/2
            # 8 vertices of box
            verts = [
                (x-hw, y-hh, z-hd), (x+hw, y-hh, z-hd), 
                (x+hw, y+hh, z-hd), (x-hw, y+hh, z-hd),
                (x-hw, y-hh, z+hd), (x+hw, y-hh, z+hd), 
                (x+hw, y+hh, z+hd), (x-hw, y+hh, z+hd)
            ]
            for v in verts:
                f.write(f"v {v[0]} {v[1]} {v[2]}\n")
            
            # Faces with proper winding
            faces = [
                (1,2,3,4), (5,8,7,6), (1,5,6,2), 
                (4,3,7,8), (1,4,8,5), (2,6,7,3)
            ]
            for face in faces:
                v1, v2, v3, v4 = [i + v_start - 1 for i in face]
                f.write(f"f {v1} {v2} {v3} {v4}\n")
            return v_start + 8
        
        v_idx = 1
        
        # LEGS (with boots)
        # Left boot
        v_idx = write_box(-0.3, 0.15, 0, 0.25, 0.3, 0.3, v_idx)
        # Right boot
        v_idx = write_box(0.3, 0.15, 0, 0.25, 0.3, 0.3, v_idx)
        
        # Left leg
        v_idx = write_box(-0.3, 0.6, 0, 0.22, 0.6, 0.22, v_idx)
        # Right leg
        v_idx = write_box(0.3, 0.6, 0, 0.22, 0.6, 0.22, v_idx)
        
        # TORSO
        # Belt
        v_idx = write_box(0, 1.0, 0, 0.7, 0.15, 0.35, v_idx)
        # Main torso
        v_idx = write_box(0, 1.4, 0, 0.65, 0.7, 0.3, v_idx)
        # Chest armor plate
        v_idx = write_box(0, 1.5, 0.16, 0.5, 0.5, 0.08, v_idx)
        
        # SHOULDERS
        # Left shoulder pad
        v_idx = write_box(-0.45, 1.75, 0, 0.3, 0.25, 0.3, v_idx)
        # Right shoulder pad
        v_idx = write_box(0.45, 1.75, 0, 0.3, 0.25, 0.3, v_idx)
        
        # ARMS
        # Left upper arm
        v_idx = write_box(-0.45, 1.35, 0, 0.18, 0.5, 0.18, v_idx)
        # Right upper arm
        v_idx = write_box(0.45, 1.35, 0, 0.18, 0.5, 0.18, v_idx)
        
        # Left forearm
        v_idx = write_box(-0.45, 0.85, 0.1, 0.16, 0.4, 0.16, v_idx)
        # Right forearm
        v_idx = write_box(0.45, 0.85, 0.1, 0.16, 0.4, 0.16, v_idx)
        
        # HANDS
        # Left hand
        v_idx = write_box(-0.45, 0.6, 0.15, 0.14, 0.2, 0.14, v_idx)
        # Right hand
        v_idx = write_box(0.45, 0.6, 0.15, 0.14, 0.2, 0.14, v_idx)
        
        # NECK
        v_idx = write_box(0, 1.85, 0, 0.18, 0.15, 0.18, v_idx)
        
        # HEAD
        # Main head
        v_idx = write_box(0, 2.1, 0, 0.35, 0.4, 0.35, v_idx)
        # Nose
        v_idx = write_box(0, 2.1, 0.2, 0.08, 0.12, 0.08, v_idx)
        
        # HAT
        # Brim
        v_idx = write_box(0, 2.4, 0, 0.7, 0.08, 0.7, v_idx)
        # Crown
        v_idx = write_box(0, 2.6, 0, 0.4, 0.3, 0.4, v_idx)
        
        # BACKPACK
        v_idx = write_box(0, 1.4, -0.25, 0.45, 0.6, 0.25, v_idx)
        # Bedroll on top
        v_idx = write_box(0, 1.9, -0.25, 0.15, 0.15, 0.4, v_idx)
        
        # ACCESSORIES
        # Water bottle on belt
        v_idx = write_box(0.4, 1.0, 0.2, 0.12, 0.25, 0.12, v_idx)
        # Pouch on belt
        v_idx = write_box(-0.35, 0.95, 0.15, 0.15, 0.15, 0.15, v_idx)

def create_pyramid(filename):
    """Create realistic Egyptian-style pyramid with detailed geometry"""
    print(f"Generating realistic pyramid model: {filename}")
    
    with open(filename, 'w') as f:
        f.write("# Realistic Egyptian Pyramid\n")
        f.write("# Base and apex with stepped detail\n\n")
        
        size = 10
        height = 8
        
        # Base vertices (square) - slightly larger for stability
        f.write(f"v {-size} 0 {-size}\n")  # 1
        f.write(f"v {size} 0 {-size}\n")   # 2
        f.write(f"v {size} 0 {size}\n")    # 3
        f.write(f"v {-size} 0 {size}\n")   # 4
        
        # Mid-level vertices for stepped appearance (optional detail)
        mid_size = size * 0.7
        mid_height = height * 0.3
        f.write(f"v {-mid_size} {mid_height} {-mid_size}\n")  # 5
        f.write(f"v {mid_size} {mid_height} {-mid_size}\n")   # 6
        f.write(f"v {mid_size} {mid_height} {mid_size}\n")    # 7
        f.write(f"v {-mid_size} {mid_height} {mid_size}\n")   # 8
        
        # Apex
        f.write(f"v 0 {height} 0\n")       # 9
        
        # Normals for proper lighting
        f.write("vn 0 -1 0\n")  # Base normal (down)
        f.write("vn 0.7071 0.7071 0\n")   # Side normals (outward + up)
        f.write("vn -0.7071 0.7071 0\n")
        f.write("vn 0 0.7071 0.7071\n")
        f.write("vn 0 0.7071 -0.7071\n")
        
        # Base face
        f.write("f 1//1 2//1 3//1 4//1\n")
        
        # Lower tier side faces (from base to mid-level)
        f.write("f 1//2 2//2 6//2 5//2\n")  # Front lower
        f.write("f 2//3 3//3 7//3 6//3\n")  # Right lower
        f.write("f 3//4 4//4 8//4 7//4\n")  # Back lower
        f.write("f 4//5 1//5 5//5 8//5\n")  # Left lower
        
        # Upper tier side faces (triangles from mid-level to apex)
        f.write("f 5//2 6//2 9//2\n")  # Front upper
        f.write("f 6//3 7//3 9//3\n")  # Right upper
        f.write("f 7//4 8//4 9//4\n")  # Back upper
        f.write("f 8//5 5//5 9//5\n")  # Left upper

def create_cactus(filename):
    """Create desert cactus"""
    print(f"Generating cactus model: {filename}")
    
    with open(filename, 'w') as f:
        f.write("# Desert Cactus\n\n")
        
        # Main trunk (cylinder approximation with 8 sides)
        trunk_height = 4
        trunk_radius = 0.4
        segments = 8
        
        # Bottom circle
        for i in range(segments):
            angle = 2 * math.pi * i / segments
            x = trunk_radius * math.cos(angle)
            z = trunk_radius * math.sin(angle)
            f.write(f"v {x} 0 {z}\n")
        
        # Top circle
        for i in range(segments):
            angle = 2 * math.pi * i / segments
            x = trunk_radius * math.cos(angle)
            z = trunk_radius * math.sin(angle)
            f.write(f"v {x} {trunk_height} {z}\n")
        
        # Side arms (smaller cylinders)
        # Left arm
        arm_y = 2.5
        arm_length = 1.5
        arm_radius = 0.25
        
        for i in range(4):
            angle = 2 * math.pi * i / 4
            y = arm_y + arm_radius * math.sin(angle)
            z = arm_radius * math.cos(angle)
            f.write(f"v {-trunk_radius} {y} {z}\n")
            f.write(f"v {-trunk_radius - arm_length} {y} {z}\n")
        
        # Faces for trunk
        for i in range(segments):
            next_i = (i + 1) % segments
            f.write(f"f {i+1} {next_i+1} {next_i+segments+1} {i+segments+1}\n")

# =============================================================================
# SOUND EFFECTS (Keep existing + add new ones)
# =============================================================================

def create_wav(filename, freq=440.0, duration=0.5):
    """Create simple WAV sound effect"""
    sample_rate = 44100
    num_samples = int(sample_rate * duration)
    
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
            value = 32767.0 * math.sin(2.0 * math.pi * freq * t) * (1.0 - t/duration)
            value = max(-32767, min(32767, int(value)))
            f.write(struct.pack('<h', value))

def create_background_music(filename, duration=30.0):
    """Create mysterious adventure music"""
    sample_rate = 44100
    num_samples = int(sample_rate * duration)
    
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
    
    # Minor key for mysterious temple vibe
    notes = {
        'A3': 220.00, 'C4': 261.63, 'D4': 293.66, 'E4': 329.63,
        'F4': 349.23, 'G4': 392.00, 'A4': 440.00, 'C5': 523.25
    }
    
    melody = ['A3', 'C4', 'E4', 'D4', 'C4', 'A3', 'G4', 'F4']
    bass = ['A3', 'A3', 'F4', 'F4']
    
    with open(filename, 'wb') as f:
        f.write(header)
        
        beat_duration = 0.6
        for i in range(num_samples):
            t = float(i) / sample_rate
            beat = int(t / beat_duration)
            
            melody_note = melody[beat % len(melody)]
            bass_note = bass[(beat // 2) % len(bass)]
            
            beat_t = (t % beat_duration) / beat_duration
            envelope = math.sin(beat_t * math.pi) * 0.8
            
            # Melody
            melody_val = 8000 * math.sin(2.0 * math.pi * notes[melody_note] * t) * envelope
            # Bass
            bass_val = 6000 * math.sin(2.0 * math.pi * notes[bass_note] * t) * 0.6
            # Ambient pad
            pad_val = 3000 * math.sin(2.0 * math.pi * notes[melody_note] * 0.5 * t)
            
            value = melody_val + bass_val + pad_val
            value = max(-32767, min(32767, int(value)))
            f.write(struct.pack('<h', value))

# =============================================================================
# MAIN EXECUTION
# =============================================================================

if __name__ == "__main__":
    print("=" * 60)
    print("ENHANCED ASSET GENERATOR")
    print("Shadow Temple Escape - Professional Edition")
    print("=" * 60)
    
    if not os.path.exists("assets"):
        os.makedirs("assets")
        print("✓ Created assets directory\n")
    
    print("\n[1/4] GENERATING PROFESSIONAL TEXTURES")
    print("-" * 60)
    
    # Ground textures
    create_sand_texture("assets/sand_ground.bmp", 512, 512)
    create_snow_texture("assets/snow_ground.bmp", 512, 512)
    
    # Wall textures
    create_sandstone_wall_texture("assets/sandstone_wall.bmp", 256, 256)
    create_ice_wall_texture("assets/ice_wall.bmp", 256, 256)
    
    # Keep old names for compatibility
    create_sand_texture("assets/ground.bmp", 512, 512)
    create_sandstone_wall_texture("assets/wall.bmp", 256, 256)
    
    print("\n[2/4] GENERATING ENHANCED 3D MODELS")
    print("-" * 60)
    
    # Enhanced player
    create_detailed_player("assets/player.obj")
    
    # Desert environment
    create_pyramid("assets/pyramid.obj")
    create_cactus("assets/cactus.obj")
    
    # Keep existing models
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
                tl = z * (tiles + 1) + x + 1
                tr = tl + 1
                bl = (z + 1) * (tiles + 1) + x + 1
                br = bl + 1
                f.write(f"f {tl}/{tl}/1 {bl}/{bl}/1 {br}/{br}/1 {tr}/{tr}/1\n")
    
    create_obj_cube("assets/pillar.obj")
    create_obj_tree("assets/tree.obj")
    create_obj_cube("assets/rock.obj")
    create_obj_ground("assets/ground.obj")
    
    print("\n[3/4] GENERATING SOUND EFFECTS")
    print("-" * 60)
    
    sound_list = [
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
    
    for sound_name, freq, dur in sound_list:
        create_wav(f"assets/{sound_name}", freq, dur)
        print(f"  ✓ {sound_name}")
    
    print("\n[4/4] GENERATING BACKGROUND MUSIC")
    print("-" * 60)
    create_background_music("assets/background.wav", duration=30.0)
    
    print("\n" + "=" * 60)
    print("✅ ASSET GENERATION COMPLETE!")
    print("=" * 60)
    print("\nGenerated Assets:")
    print("  📁 Textures:")
    print("     • sand_ground.bmp (Professional sand texture)")
    print("     • snow_ground.bmp (Professional snow texture)")
    print("     • sandstone_wall.bmp (Desert walls)")
    print("     • ice_wall.bmp (Ice cave walls)")
    print("\n  📁 3D Models:")
    print("     • player.obj (Detailed adventurer)")
    print("     • pyramid.obj (Egyptian pyramid)")
    print("     • cactus.obj (Desert cactus)")
    print("     • pillar.obj, tree.obj, rock.obj, ground.obj")
    print("\n  📁 Audio:")
    print("     • 9 sound effects + background music")
    print("\n" + "=" * 60)
    print("Ready for professional gameplay! 🎮")
    print("=" * 60)