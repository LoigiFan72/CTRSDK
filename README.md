# CTRSDK-4_2_5

A WIP rewrite of the CTR SDK used in the Mario & Luigi: Dream Team US 1.0 decompilation.

# DISCLAIMER:

Some of the functions I rewrited in ASM, or Asmebly code (Mispell). *Why?*

I'm lazy to be simple, but the rules for that are stated down below;

# Rewrite rules:

1) Any game function that calls into an SDK function will have that SDK function rewritten alongside it. If a function makes no SDK calls, inline ASM is a valid alternative. (nn::math, and svc are exempt.)

2) Any ASM function will explain what it does.