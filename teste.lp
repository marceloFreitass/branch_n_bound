\ Model bruno_bruck
\ LP format - for model browsing. Use MPS format to capture full model detail.
\ Signature: 0x25388bad770d8dfb
Maximize
  9 x1 + 7 x2 + 10 x3 + 7 x4 + 9 x5 + 6 x6 + 8 x7 + 4 x8 + 9 x9
Subject To
 R0: 4 x1 + 9 x2 + 4 x3 + x4 + 9 x5 + 6 x6 + 3 x7 + 6 x8 + x9 <= 40
 R1: 3 x1 + 7 x2 + 8 x3 + 7 x4 + 6 x5 + 3 x6 + 5 x7 + 9 x8 + 4 x9 <= 80
 R2: 9 x1 + 3 x2 + 6 x3 + 5 x4 + 7 x5 + x6 + x7 + 3 x8 + 9 x9 <= 40
 R3: 5 x1 + 9 x2 + 6 x3 + 5 x4 + 9 x5 + 7 x6 + 8 x7 + 7 x8 + 8 x9 <= 10
 R4: 7 x1 + 7 x2 + 4 x3 + x4 + 3 x5 + 4 x6 + 8 x7 + x8 + 9 x9 <= 10
 R5: x1 + 6 x2 + 6 x3 + x4 + 6 x5 + 7 x6 + 3 x7 + 8 x8 + 7 x9 <= 10
 R6: 6 x1 + 6 x2 + 8 x3 + 6 x4 + 10 x5 + 8 x6 + x7 + 4 x8 + 4 x9 <= 70
 R7: 9 x1 + x2 + 9 x3 + 7 x4 + 10 x5 + 5 x6 + 6 x7 + 2 x8 + 5 x9 <= 10
 R8: 2 x1 + 7 x2 + 6 x3 + 5 x4 + x5 + x6 + 9 x7 + 2 x8 + x9 <= 20
Bounds
 x1 <= 1
 x2 <= 1
 x3 <= 1
 x4 <= 1
 x5 <= 1
 x6 <= 1
 x7 <= 1
 x8 <= 1
 x9 <= 1
End
