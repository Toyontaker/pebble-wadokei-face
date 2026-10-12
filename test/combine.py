import sys
from PIL import Image
out_path, paths = sys.argv[1], sys.argv[2:]
ims = [Image.open(p).convert('RGB') for p in paths]
ims = [i.resize((i.width * 2, i.height * 2), Image.NEAREST) for i in ims]
W = sum(i.width for i in ims) + 10 * len(ims); H = max(i.height for i in ims)
out = Image.new('RGB', (W, H), (128, 128, 128)); x = 0
for i in ims:
    out.paste(i, (x, 0)); x += i.width + 10
out.save(out_path)
