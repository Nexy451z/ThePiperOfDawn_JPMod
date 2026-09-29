import os
import sys
import json
import struct
import base64
import hashlib
import hmac
import UnityPy
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import serialization

sys.stdout.reconfigure(encoding='utf-8')

GAME_DIR = r"C:\Program Files (x86)\Steam\steamapps\common\The Piper Of Dawn"
YOO_DIR = os.path.join(GAME_DIR, "ThePiper_Data", "StreamingAssets", "yoo", "Main")
OUT_DIR = r"C:\Users\haruk\.gemini\antigravity\scratch\ThePiperOfDawn_JPMod\extracted"
os.makedirs(OUT_DIR, exist_ok=True)

# 1. Master Key Derivation (from Foundation.Crypto.BundleCryptoKey)
R14 = [
    bytes.fromhex('32a0539dd17f72d9'),
    bytes.fromhex('e91b00538ebbad4b'),
    bytes.fromhex('c8666a6ac4032cef'),
    bytes.fromhex('883b4194f349a56b'),
]
R15 = [
    bytes.fromhex('22861d9f1a344322'),
    bytes.fromhex('70de554ed7ff6e34'),
    bytes.fromhex('a17ac68433bedf8b'),
    bytes.fromhex('cf1577cd99b688d7'),
]
ORDER = [2, 0, 3, 1]
raw_seed = bytearray(a ^ b for idx in ORDER for a, b in zip(R14[idx], R15[idx]))
MASTER_KEY = hashlib.sha256(raw_seed).digest()

# 2. RSA-1024 Public Key (for first 128 bytes of table .bytes assets)
PUB_DER = base64.b64decode(
    "MIGdMA0GCSqGSIb3DQEBAQUAA4GLADCBhwKBgQCyJiTzABL2wironv9+4wnZTg7J"
    "Xr1ekiMA3RdL2e+W8kEtyZgghb5KBBAASKuiGNxhadrnSgC8+h1r7B/JLudatvdl"
    "zwyy1gAs/mbVYHd7x1WoBfzDpWkZX8bhDO/uX4GnBhWAmtapbbjVGOAVIuaIV8lB"
    "zNXJ30mJPDI4wKc7/QIBAw=="
)
PUB_KEY = serialization.load_der_public_key(PUB_DER)
PUB_NUMS = PUB_KEY.public_numbers()


def decrypt_bundle(bundle_name: str, filename: str) -> bytes:
    path = os.path.join(YOO_DIR, filename)
    key = hmac.new(MASTER_KEY, ('bundle-key|' + bundle_name).encode('utf-8'), hashlib.sha256).digest()
    iv = hmac.new(MASTER_KEY, ('bundle-iv|' + bundle_name).encode('utf-8'), hashlib.sha256).digest()[:8] + b'\x00' * 8
    with open(path, 'rb') as f:
        enc = f.read()
    # Note: AesCtrKeystream resets/increments counter per 16-byte block across the whole file
    return Cipher(algorithms.AES(key), modes.CTR(iv)).decryptor().update(enc)


def rsa_public_decrypt(block128: bytes) -> bytes:
    c = int.from_bytes(block128, 'big')
    m = pow(c, PUB_NUMS.e, PUB_NUMS.n)
    raw = m.to_bytes(128, 'big')
    if raw[0] == 0 and raw[1] in (1, 2):
        sep = raw.find(b'\x00', 2)
        if sep != -1:
            return raw[sep + 1:]
    return raw


def decrypt_table_bytes(data_bytes: bytes) -> bytes:
    if len(data_bytes) < 128:
        return data_bytes
    head = rsa_public_decrypt(data_bytes[:128])
    res = bytearray(head) + bytearray(data_bytes[128:])
    idx, step = 1, 0
    while idx < len(res):
        res[idx] ^= (idx - step) & 0xff
        idx *= 2
        step += 1
    idx, step = len(res) - 1, 0
    while idx > 0:
        res[idx] ^= (idx - step) & 0xff
        idx //= 2
        step += 1
    return bytes(res)


# Minimal Protobuf wire format parser
def read_varint(buf: bytes, pos: int):
    result = 0
    shift = 0
    while True:
        b = buf[pos]
        pos += 1
        result |= (b & 0x7f) << shift
        if not (b & 0x80):
            return result, pos
        shift += 7


def parse_proto_fields(buf: bytes, start: int = 0, end: int = None):
    if end is None:
        end = len(buf)
    pos = start
    fields = []
    while pos < end:
        tag_wire, pos = read_varint(buf, pos)
        field_num = tag_wire >> 3
        wire_type = tag_wire & 0x7
        if wire_type == 0:  # varint
            val, pos = read_varint(buf, pos)
            fields.append((field_num, wire_type, val))
        elif wire_type == 2:  # length-delimited
            length, pos = read_varint(buf, pos)
            val = buf[pos:pos + length]
            pos += length
            fields.append((field_num, wire_type, val))
        elif wire_type == 5:  # 32-bit
            val = buf[pos:pos + 4]
            pos += 4
            fields.append((field_num, wire_type, val))
        elif wire_type == 1:  # 64-bit
            val = buf[pos:pos + 8]
            pos += 8
            fields.append((field_num, wire_type, val))
        else:
            raise ValueError(f"Unknown wire_type {wire_type} at pos {pos}")
    return fields


def parse_language_table(dec_bytes: bytes):
    count = struct.unpack('<i', dec_bytes[:4])[0]
    top_fields = parse_proto_fields(dec_bytes, 4)
    entries = []
    for fnum, wtype, val in top_fields:
        if fnum == 2 and wtype == 2:  # repeated Language
            sub = parse_proto_fields(val)
            item = {"id": 0, "cn": "", "tw": "", "en": "", "jp": ""}
            for sfnum, swtype, sval in sub:
                if sfnum == 1:
                    item["id"] = sval
                elif sfnum == 2:
                    item["cn"] = sval.decode('utf-8', errors='replace')
                elif sfnum == 3:
                    item["tw"] = sval.decode('utf-8', errors='replace')
                elif sfnum == 4:
                    item["en"] = sval.decode('utf-8', errors='replace')
                elif sfnum == 5:
                    item["jp"] = sval.decode('utf-8', errors='replace')
            entries.append(item)
    return count, entries


def parse_language_talk_table(dec_bytes: bytes):
    count = struct.unpack('<i', dec_bytes[:4])[0]
    top_fields = parse_proto_fields(dec_bytes, 4)
    entries = []
    for fnum, wtype, val in top_fields:
        if fnum == 2 and wtype == 2:  # repeated LanguageTalk
            sub = parse_proto_fields(val)
            item = {"id": 0, "cn": "", "flag": 0, "tw": "", "en": "", "jp": ""}
            for sfnum, swtype, sval in sub:
                if sfnum == 1:
                    item["id"] = sval
                elif sfnum == 2:
                    item["cn"] = sval.decode('utf-8', errors='replace')
                elif sfnum == 3:
                    item["flag"] = sval
                elif sfnum == 4:
                    item["tw"] = sval.decode('utf-8', errors='replace')
                elif sfnum == 5:
                    item["en"] = sval.decode('utf-8', errors='replace')
                elif sfnum == 6:
                    item["jp"] = sval.decode('utf-8', errors='replace')
            entries.append(item)
    return count, entries


def main():
    print("Decrypting assets_gameres_table.bundle...")
    table_bundle = decrypt_bundle("assets_gameres_table.bundle", "c9c85bb2d9cbae829d7118a9c414bae6.bundle")
    env = UnityPy.load(table_bundle)

    raw_tables_dir = os.path.join(OUT_DIR, "raw_tables")
    os.makedirs(raw_tables_dir, exist_ok=True)

    for obj in env.objects:
        if obj.type.name == "TextAsset":
            data = obj.read()
            name = data.m_Name
            raw_b = data.m_Script.encode('utf-8', 'surrogateescape')
            try:
                dec_b = decrypt_table_bytes(raw_b)
            except Exception as e:
                print(f"  [WARN] Could not decrypt {name}: {e}")
                dec_b = raw_b

            with open(os.path.join(raw_tables_dir, f"{name}.bytes"), "wb") as f:
                f.write(dec_b)

            if name == "Language":
                cnt, entries = parse_language_table(dec_b)
                print(f"Parsed Language: header_count={cnt}, extracted={len(entries)}")
                out_path = os.path.join(OUT_DIR, "Language.json")
                with open(out_path, "w", encoding="utf-8") as f:
                    json.dump(entries, f, ensure_ascii=False, indent=2)
            elif name == "LanguageTalk":
                cnt, entries = parse_language_talk_table(dec_b)
                print(f"Parsed LanguageTalk: header_count={cnt}, extracted={len(entries)}")
                out_path = os.path.join(OUT_DIR, "LanguageTalk.json")
                with open(out_path, "w", encoding="utf-8") as f:
                    json.dump(entries, f, ensure_ascii=False, indent=2)

    print("Decrypting assets_gameres_story.bundle...")
    story_bundle = decrypt_bundle("assets_gameres_story.bundle", "c634cb826cbd3b5cc049319092b742d2.bundle")
    story_env = UnityPy.load(story_bundle)
    raw_story_dir = os.path.join(OUT_DIR, "raw_story")
    os.makedirs(raw_story_dir, exist_ok=True)
    story_count = 0
    for obj in story_env.objects:
        if obj.type.name == "TextAsset":
            data = obj.read()
            name = data.m_Name
            raw_b = data.m_Script.encode('utf-8', 'surrogateescape')
            with open(os.path.join(raw_story_dir, f"{name}.bytes"), "wb") as f:
                f.write(raw_b)
            story_count += 1
    print(f"Extracted {story_count} story files to {raw_story_dir}")


if __name__ == "__main__":
    main()
