        .include "MPlayDef.s"

        .equ    kururinMapOctopusGarden_002_grp, voicegroup000
        .equ    kururinMapOctopusGarden_002_pri, 0
        .equ    kururinMapOctopusGarden_002_rev, 50+reverb_set
        .equ    kururinMapOctopusGarden_002_key, 0

        .section .rodata
        .global kururinMapOctopusGarden_002
        .align  2

@****************** Track 0 (Midi-Chn.0) ******************@

kururinMapOctopusGarden_002_0:
        .byte   KEYSH , kururinMapOctopusGarden_002_key+0
@ 000   ----------------------------------------
        .byte   TEMPO , 170/2
kururinMapOctopusGarden_002_0_LOOP:
        .byte           VOICE , 2
        .byte           VOL   , 110
        .byte           N28   , Dn3 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N28   , An2 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
@ 001   ----------------------------------------
kururinMapOctopusGarden_002_0_1:
        .byte           N28   , Dn3 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N28   , An2 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte   PEND
@ 002   ----------------------------------------
kururinMapOctopusGarden_002_0_2:
        .byte           N28   , Bn2 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N28   , Fs2 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte   PEND
@ 003   ----------------------------------------
kururinMapOctopusGarden_002_0_3:
        .byte           N28   , Bn2 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N23   , Fs3
        .byte   W24
        .byte                   Fn3
        .byte   W24
        .byte   PEND
@ 004   ----------------------------------------
kururinMapOctopusGarden_002_0_4:
        .byte           N28   , En3 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N28   , Bn2 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte   PEND
@ 005   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_0_4
@ 006   ----------------------------------------
        .byte           N28   , An3 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N28   , En3 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
@ 007   ----------------------------------------
        .byte           N23   , An3
        .byte   W40
        .byte           N30   , En3 , v127 , gtp1
        .byte   W32
        .byte           N07   , An2
        .byte   W08
        .byte                   Bn2
        .byte   W08
        .byte                   Cs3
        .byte   W08
@ 008   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_0_1
@ 009   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_0_1
@ 010   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_0_2
@ 011   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_0_3
@ 012   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_0_4
@ 013   ----------------------------------------
        .byte           N28   , An3 , v127 , gtp1
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N23   , En3
        .byte   W24
        .byte                   An2
        .byte   W24
@ 014   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_0_1
@ 015   ----------------------------------------
        .byte           N23   , Dn3 , v127
        .byte   W24
        .byte           N22   , An2
        .byte   W24
        .byte                   Bn2
        .byte   W24
        .byte                   Cs3
        .byte   W24
@ 016   ----------------------------------------
        .byte   GOTO
         .word  kururinMapOctopusGarden_002_0_LOOP
        .byte   FINE

@****************** Track 1 (Midi-Chn.1) ******************@

kururinMapOctopusGarden_002_1:
        .byte   KEYSH , kururinMapOctopusGarden_002_key+0
@ 000   ----------------------------------------
kururinMapOctopusGarden_002_1_LOOP:
        .byte           VOICE , 127
        .byte           VOL   , 110
        .byte           N24   , EnM2 , v108
        .byte   W24
        .byte                   Fs2
        .byte   W24
        .byte           N16   , EnM2
        .byte   W16
        .byte           N08   , EnM2 , v092
        .byte   W08
        .byte           N24   , Fs2 , v108
        .byte   W24
@ 001   ----------------------------------------
kururinMapOctopusGarden_002_1_1:
        .byte           N24   , EnM2 , v108
        .byte   W24
        .byte                   Fs2
        .byte   W24
        .byte           N16   , EnM2
        .byte   W16
        .byte           N08   , EnM2 , v092
        .byte   W08
        .byte           N24   , Fs2 , v108
        .byte   W24
        .byte   PEND
@ 002   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 003   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 004   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 005   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 006   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 007   ----------------------------------------
        .byte           N40   , Fs2 , v108
        .byte   W40
        .byte           N32   , EnM2
        .byte   W32
        .byte           N08   , Fs2 , v092
        .byte   W08
        .byte                   Fs2
        .byte   W08
        .byte                   Fs2
        .byte   W08
@ 008   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 009   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 010   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 011   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 012   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 013   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 014   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_1_1
@ 015   ----------------------------------------
        .byte           N24   , Fs2 , v108
        .byte   W24
        .byte                   EnM2
        .byte   W24
        .byte                   EnM2
        .byte   W24
        .byte           N08   , Fs2 , v092
        .byte   W08
        .byte                   Fs2
        .byte   W08
        .byte                   Fs2
        .byte   W08
@ 016   ----------------------------------------
        .byte   GOTO
         .word  kururinMapOctopusGarden_002_1_LOOP
        .byte   FINE

@****************** Track 2 (Midi-Chn.2) ******************@

kururinMapOctopusGarden_002_2:
        .byte   KEYSH , kururinMapOctopusGarden_002_key+0
@ 000   ----------------------------------------
kururinMapOctopusGarden_002_2_LOOP:
        .byte           VOICE , 72
        .byte           VOL   , 110
        .byte           PAN   , c_v-64
        .byte           TUNE  , c_v+14
        .byte   W24
        .byte           N10   , Fs3 , v080
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W16
        .byte           N07   , An2
        .byte   W08
@ 001   ----------------------------------------
kururinMapOctopusGarden_002_2_1:
        .byte   W24
        .byte           N10   , Fs3 , v080
        .byte   W40
        .byte           N07
        .byte   W24
        .byte                   Fs3
        .byte   W08
        .byte   PEND
@ 002   ----------------------------------------
kururinMapOctopusGarden_002_2_2:
        .byte   W24
        .byte           N10   , Fs3 , v080
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W16
        .byte           N07   , Bn2
        .byte   W08
        .byte   PEND
@ 003   ----------------------------------------
kururinMapOctopusGarden_002_2_3:
        .byte   W24
        .byte           N10   , Fs3 , v080
        .byte   W24
        .byte           N23
        .byte   W24
        .byte                   Fn3
        .byte   W24
        .byte   PEND
@ 004   ----------------------------------------
kururinMapOctopusGarden_002_2_4:
        .byte   W24
        .byte           N10   , Gn3 , v080
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W16
        .byte           N07   , Bn2
        .byte   W08
        .byte   PEND
@ 005   ----------------------------------------
        .byte   W24
        .byte           N10   , Gn3
        .byte   W40
        .byte           N07
        .byte   W24
        .byte                   Gn3
        .byte   W08
@ 006   ----------------------------------------
        .byte   W24
        .byte           N10   , An3
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W16
        .byte           N07   , En2
        .byte   W08
@ 007   ----------------------------------------
        .byte           N10   , En3
        .byte   W40
        .byte           N54   , An3 , v080 , gtp1
        .byte   W56
@ 008   ----------------------------------------
kururinMapOctopusGarden_002_2_8:
        .byte   W24
        .byte           N10   , Fs3 , v080
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W16
        .byte           N07   , An2
        .byte   W08
        .byte   PEND
@ 009   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_2_1
@ 010   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_2_2
@ 011   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_2_3
@ 012   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_2_4
@ 013   ----------------------------------------
        .byte   W24
        .byte           N10   , An3 , v080
        .byte   W40
        .byte           N07
        .byte   W24
        .byte                   An3
        .byte   W08
@ 014   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_2_8
@ 015   ----------------------------------------
        .byte           N15   , Fs3 , v080
        .byte   W24
        .byte                   An3
        .byte   W24
        .byte                   Bn3
        .byte   W24
        .byte                   Cs4
        .byte   W24
@ 016   ----------------------------------------
        .byte   GOTO
         .word  kururinMapOctopusGarden_002_2_LOOP
        .byte   FINE

@****************** Track 3 (Midi-Chn.3) ******************@

kururinMapOctopusGarden_002_3:
        .byte   KEYSH , kururinMapOctopusGarden_002_key+0
@ 000   ----------------------------------------
kururinMapOctopusGarden_002_3_LOOP:
        .byte           VOICE , 57
        .byte           VOL   , 110
        .byte           PAN   , c_v-64
        .byte           TUNE  , c_v+14
        .byte   W24
        .byte           N10   , Dn3 , v080
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W24
@ 001   ----------------------------------------
kururinMapOctopusGarden_002_3_1:
        .byte   W24
        .byte           N10   , Dn3 , v080
        .byte   W40
        .byte           N07
        .byte   W24
        .byte                   Dn3
        .byte   W08
        .byte   PEND
@ 002   ----------------------------------------
kururinMapOctopusGarden_002_3_2:
        .byte   W24
        .byte           N10   , Dn3 , v080
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W24
        .byte   PEND
@ 003   ----------------------------------------
kururinMapOctopusGarden_002_3_3:
        .byte   W24
        .byte           N10   , Dn3 , v080
        .byte   W24
        .byte           N23
        .byte   W24
        .byte                   Dn3
        .byte   W24
        .byte   PEND
@ 004   ----------------------------------------
kururinMapOctopusGarden_002_3_4:
        .byte   W24
        .byte           N10   , En3 , v080
        .byte   W40
        .byte           N07
        .byte   W08
        .byte           N15
        .byte   W24
        .byte   PEND
@ 005   ----------------------------------------
kururinMapOctopusGarden_002_3_5:
        .byte   W24
        .byte           N10   , En3 , v080
        .byte   W40
        .byte           N07
        .byte   W24
        .byte                   En3
        .byte   W08
        .byte   PEND
@ 006   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_4
@ 007   ----------------------------------------
        .byte           N10   , Cs3 , v080
        .byte   W40
        .byte           N54   , En3 , v080 , gtp1
        .byte   W56
@ 008   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_2
@ 009   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_1
@ 010   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_2
@ 011   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_3
@ 012   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_4
@ 013   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_5
@ 014   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_3_2
@ 015   ----------------------------------------
        .byte           N15   , Dn3 , v080
        .byte   W24
        .byte                   Fs3
        .byte   W24
        .byte                   Fs3
        .byte   W24
        .byte                   An3
        .byte   W24
@ 016   ----------------------------------------
        .byte   GOTO
         .word  kururinMapOctopusGarden_002_3_LOOP
        .byte   FINE

@****************** Track 4 (Midi-Chn.4) ******************@

kururinMapOctopusGarden_002_4:
        .byte   KEYSH , kururinMapOctopusGarden_002_key+0
@ 000   ----------------------------------------
kururinMapOctopusGarden_002_4_LOOP:
        .byte           VOL   , 110
        .byte   W96
@ 001   ----------------------------------------
        .byte   W12
        .byte           VOICE , 17
        .byte   W12
        .byte           N23   , An3 , v082
        .byte   W24
        .byte                   Dn4
        .byte   W24
        .byte                   En4
        .byte   W24
@ 002   ----------------------------------------
kururinMapOctopusGarden_002_4_2:
        .byte           N40   , Fs4 , v082
        .byte   W48
        .byte           N23   , Bn3
        .byte   W24
        .byte           N15   , Cs4
        .byte   W16
        .byte           N88   , Dn4
        .byte   W08
        .byte   PEND
@ 003   ----------------------------------------
        .byte   W96
@ 004   ----------------------------------------
        .byte   W96
@ 005   ----------------------------------------
        .byte   W24
        .byte           N23   , Bn3
        .byte   W24
        .byte                   En4
        .byte   W24
        .byte                   Fs4
        .byte   W24
@ 006   ----------------------------------------
        .byte           N40   , Gn4
        .byte   W48
        .byte           N15   , Fs4
        .byte   W16
        .byte           N08   , Gn4
        .byte   W08
        .byte           N15   , Fs4
        .byte   W16
        .byte           N13   , En4
        .byte   W08
@ 007   ----------------------------------------
        .byte   W36
        .byte   W03
        .byte           N02   , Gs4 , v066
        .byte   W02
        .byte           N54   , An4
        .byte   W54
        .byte   W01
@ 008   ----------------------------------------
        .byte           N20   , An4 , v010
        .byte   W96
@ 009   ----------------------------------------
        .byte   W24
        .byte           N23   , An3 , v082
        .byte   W24
        .byte                   Dn4
        .byte   W24
        .byte                   En4
        .byte   W24
@ 010   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_4_2
@ 011   ----------------------------------------
        .byte   W96
@ 012   ----------------------------------------
        .byte   W96
@ 013   ----------------------------------------
        .byte   W24
        .byte           N23   , An3 , v082
        .byte   W24
        .byte                   Cs4
        .byte   W24
        .byte                   En4
        .byte   W24
@ 014   ----------------------------------------
        .byte           N15
        .byte   W16
        .byte           N08   , Fs4
        .byte   W08
        .byte           N15   , En4
        .byte   W16
        .byte           N56   , Dn4
        .byte   W56
@ 015   ----------------------------------------
        .byte   W96
@ 016   ----------------------------------------
        .byte           VOICE , 19
        .byte                   0
        .byte   GOTO
         .word  kururinMapOctopusGarden_002_4_LOOP
        .byte   FINE

@****************** Track 5 (Midi-Chn.5) ******************@

kururinMapOctopusGarden_002_5:
        .byte   KEYSH , kururinMapOctopusGarden_002_key+0
@ 000   ----------------------------------------
kururinMapOctopusGarden_002_5_LOOP:
        .byte           VOICE , 27
        .byte           VOL   , 95
        .byte           PAN   , c_v+45
        .byte   W96
@ 001   ----------------------------------------
        .byte   W96
@ 002   ----------------------------------------
        .byte   W96
@ 003   ----------------------------------------
        .byte   W24
        .byte           N23   , Bn4 , v088
        .byte   W24
        .byte                   Cs5
        .byte   W24
        .byte                   Dn5
        .byte   W24
@ 004   ----------------------------------------
        .byte                   En5
        .byte   W96
@ 005   ----------------------------------------
        .byte   W96
@ 006   ----------------------------------------
        .byte   W96
@ 007   ----------------------------------------
        .byte   W96
@ 008   ----------------------------------------
        .byte   W96
@ 009   ----------------------------------------
        .byte   W96
@ 010   ----------------------------------------
        .byte   W96
@ 011   ----------------------------------------
        .byte   W24
        .byte                   Bn4
        .byte   W24
        .byte                   Fs5
        .byte   W24
        .byte                   Dn5
        .byte   W24
@ 012   ----------------------------------------
        .byte                   En5
        .byte   W96
@ 013   ----------------------------------------
        .byte   W96
@ 014   ----------------------------------------
        .byte   W96
@ 015   ----------------------------------------
        .byte   W96
@ 016   ----------------------------------------
        .byte   GOTO
         .word  kururinMapOctopusGarden_002_5_LOOP
        .byte   FINE

@****************** Track 6 (Midi-Chn.6) ******************@

kururinMapOctopusGarden_002_6:
        .byte   KEYSH , kururinMapOctopusGarden_002_key+0
@ 000   ----------------------------------------
kururinMapOctopusGarden_002_6_LOOP:
        .byte           VOL   , 110
        .byte   W20
        .byte           VOICE , 19
        .byte   W04
        .byte           N23   , An4 , v096
        .byte           N23   , Fs4
        .byte   W24
        .byte                   An4
        .byte           N23   , Fs4
        .byte   W24
        .byte           N15   , Bn4
        .byte           N15   , Gn4
        .byte   W16
        .byte           N30   , An4 , v096 , gtp1
        .byte                   Fs4
        .byte   W08
@ 001   ----------------------------------------
kururinMapOctopusGarden_002_6_1:
        .byte   W40
        .byte           VOICE , 17
        .byte   W04
        .byte           N23   , An3 , v026
        .byte   W24
        .byte                   Dn4
        .byte   W24
        .byte                   En4
        .byte   W04
        .byte   PEND
@ 002   ----------------------------------------
        .byte   W20
        .byte           VOICE , 19
        .byte   W04
        .byte           N23   , Dn4 , v096
        .byte           N23   , Fs4
        .byte   W24
        .byte           N15   , Dn4
        .byte           N15   , Fs4
        .byte   W16
        .byte                   Gn4
        .byte           N15   , En4
        .byte   W24
        .byte           N30   , Dn4 , v096 , gtp1
        .byte                   Fs4
        .byte   W08
@ 003   ----------------------------------------
kururinMapOctopusGarden_002_6_3:
        .byte   W72
        .byte           N15   , Fs4 , v096
        .byte   W16
        .byte           N07   , En4
        .byte   W08
        .byte   PEND
@ 004   ----------------------------------------
kururinMapOctopusGarden_002_6_4:
        .byte           N23   , Dn4 , v096
        .byte           N23   , Bn3
        .byte   W24
        .byte                   Dn4
        .byte           N23   , Bn3
        .byte   W24
        .byte           N15   , Dn4
        .byte           N15   , Bn3
        .byte   W16
        .byte                   Bn4
        .byte   W24
        .byte           N23   , An4
        .byte           N23   , Fs4
        .byte   W08
        .byte   PEND
@ 005   ----------------------------------------
kururinMapOctopusGarden_002_6_5:
        .byte   W24
        .byte           N23   , Dn4 , v096
        .byte           N23   , Bn3 , v076
        .byte   W40
        .byte           N07   , Dn4 , v096
        .byte   W08
        .byte           N15   , Fs4
        .byte           N15   , Dn4 , v076
        .byte   W16
        .byte                   En4 , v096
        .byte           N15   , Cs4
        .byte   W08
        .byte   PEND
@ 006   ----------------------------------------
        .byte   W16
        .byte           VOICE , 17
        .byte   W04
        .byte           N40   , Gn4 , v026
        .byte   W48
        .byte           N15   , Fs4
        .byte   W16
        .byte           N08   , Gn4
        .byte   W08
        .byte           N15   , Fs4
        .byte   W04
@ 007   ----------------------------------------
        .byte   W12
        .byte           N13   , En4
        .byte   W24
        .byte   W03
        .byte           N02   , Ds4 , v066
        .byte   W02
        .byte           N54   , En4
        .byte   W54
        .byte   W01
@ 008   ----------------------------------------
        .byte           N20   , En4 , v010
        .byte   W20
        .byte           VOICE , 19
        .byte   W04
        .byte           N23   , An4 , v096
        .byte           N23   , Fs4
        .byte   W24
        .byte           N15   , An4
        .byte           N15   , Fs4
        .byte   W16
        .byte                   Bn4
        .byte           N15   , Gn4
        .byte   W24
        .byte           N30   , An4 , v096 , gtp1
        .byte                   Fs4
        .byte   W08
@ 009   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_6_1
@ 010   ----------------------------------------
        .byte   W20
        .byte           VOICE , 19
        .byte   W04
        .byte           N23   , Dn4 , v096
        .byte           N23   , Fs4
        .byte   W24
        .byte                   Dn4
        .byte           N23   , Fs4
        .byte   W24
        .byte           N15   , Gn4
        .byte           N15   , En4
        .byte   W16
        .byte           N30   , Dn4 , v096 , gtp1
        .byte                   Fs4
        .byte   W08
@ 011   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_6_3
@ 012   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_6_4
@ 013   ----------------------------------------
        .byte   PATT
         .word  kururinMapOctopusGarden_002_6_5
@ 014   ----------------------------------------
        .byte   W16
        .byte           VOICE , 17
        .byte   W04
        .byte           N15   , En4 , v026
        .byte   W16
        .byte           N08   , Fs4
        .byte   W08
        .byte           N15   , En4
        .byte   W16
        .byte           N56   , Dn4
        .byte   W36
@ 015   ----------------------------------------
        .byte   W20
        .byte           VOICE , 19
        .byte   W04
        .byte           N22   , An4 , v096
        .byte           N22   , Fs4
        .byte   W24
        .byte                   Bn4
        .byte           N22   , Fs4
        .byte   W24
        .byte                   Cs5
        .byte           N22   , An4
        .byte   W24
@ 016   ----------------------------------------
        .byte           VOICE , 17
        .byte                   19
        .byte                   0
        .byte   GOTO
         .word  kururinMapOctopusGarden_002_6_LOOP
        .byte   FINE


@********************** End of Song ***********************@

        .align  2
kururinMapOctopusGarden_002:
        .byte   7                       @ Num Tracks
        .byte   0                       @ Unknown
        .byte   kururinMapOctopusGarden_002_pri @ Priority
        .byte   kururinMapOctopusGarden_002_rev @ Reverb

        .word   kururinMapOctopusGarden_002_grp

        .word   kururinMapOctopusGarden_002_0
        .word   kururinMapOctopusGarden_002_1
        .word   kururinMapOctopusGarden_002_2
        .word   kururinMapOctopusGarden_002_3
        .word   kururinMapOctopusGarden_002_4
        .word   kururinMapOctopusGarden_002_5
        .word   kururinMapOctopusGarden_002_6

        .end
