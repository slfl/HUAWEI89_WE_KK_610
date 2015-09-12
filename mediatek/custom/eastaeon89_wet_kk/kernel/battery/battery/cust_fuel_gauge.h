#include <mach/mt_typedefs.h>

#define TEMPERATURE_T0                  110
#define TEMPERATURE_T1                  0
#define TEMPERATURE_T2                  25
#define TEMPERATURE_T3                  50
#define TEMPERATURE_T                   255 // This should be fixed, never change the value

#define FG_METER_RESISTANCE 	0

#define MAX_BOOTING_TIME_FGCURRENT	1*10 // 10s

#if defined(CONFIG_POWER_EXT)
#define OCV_BOARD_COMPESATE	72 //mV 
#define R_FG_BOARD_BASE		1000
#define R_FG_BOARD_SLOPE	1000 //slope
#else
#define OCV_BOARD_COMPESATE	0 //mV 
#define R_FG_BOARD_BASE		1000
#define R_FG_BOARD_SLOPE	1000 //slope
#endif

#define Q_MAX_POS_50	2200
#define Q_MAX_POS_25	2185
#define Q_MAX_POS_0		2129
#define Q_MAX_NEG_10	2017

#define Q_MAX_POS_50_H_CURRENT	2175
#define Q_MAX_POS_25_H_CURRENT	2170
#define Q_MAX_POS_0_H_CURRENT	2092
#define Q_MAX_NEG_10_H_CURRENT	1917

#define R_FG_VALUE 				20 // mOhm, base is 20
#define CURRENT_DETECT_R_FG	10  //1mA

#define OSR_SELECT_7			0

#define CAR_TUNE_VALUE			100 //1.00

/////////////////////////////////////////////////////////////////////
// <DOD, Battery_Voltage> Table
/////////////////////////////////////////////////////////////////////
typedef struct _BATTERY_PROFILE_STRUC
{
    kal_int32 percentage;
    kal_int32 voltage;
} BATTERY_PROFILE_STRUC, *BATTERY_PROFILE_STRUC_P;

typedef enum
{
    T1_0C,
    T2_25C,
    T3_50C
} PROFILE_TEMPERATURE;

// T0 -10C
BATTERY_PROFILE_STRUC battery_profile_t0[] =
{
	{	0	,	4177	},
	{	2	,	4162	},
	{	3	,	4145	},
	{	5	,	4118	},
	{	6	,	4091	},
	{	8	,	4071	},
	{	9	,	4052	},
	{	11	,	4032	},
	{	12	,	4012	},
	{	14	,	3996	},
	{	16	,	3986	},
	{	17	,	3978	},
	{	19	,	3969	},
	{	20	,	3961	},
	{	22	,	3953	},
	{	23	,	3945	},
	{	25	,	3936	},
	{	26	,	3926	},
	{	28	,	3916	},
	{	30	,	3904	},
	{	31	,	3894	},
	{	33	,	3882	},
	{	34	,	3872	},
	{	36	,	3862	},
	{	37	,	3854	},
	{	39	,	3847	},
	{	40	,	3839	},
	{	42	,	3833	},
	{	44	,	3827	},
	{	45	,	3823	},
	{	47	,	3817	},
	{	48	,	3813	},
	{	50	,	3808	},
	{	51	,	3804	},
	{	53	,	3801	},
	{	55	,	3797	},
	{	56	,	3794	},
	{	58	,	3791	},
	{	59	,	3788	},
	{	61	,	3784	},
	{	62	,	3782	},
	{	64	,	3780	},
	{	65	,	3778	},
	{	67	,	3776	},
	{	68	,	3773	},
	{	70	,	3770	},
	{	72	,	3766	},
	{	73	,	3763	},
	{	75	,	3759	},
	{	76	,	3754	},
	{	78	,	3749	},
	{	79	,	3744	},
	{	81	,	3738	},
	{	83	,	3731	},
	{	84	,	3724	},
	{	86	,	3717	},
	{	87	,	3711	},
	{	89	,	3707	},
	{	90	,	3702	},
	{	92	,	3695	},
	{	93	,	3678	},
	{	95	,	3646	},
	{	97	,	3595	},
	{	98	,	3521	},
	{	99	,	3474	},
	{	99	,	3453	},
	{	99	,	3441	},
	{	100	,	3433	},
	{	100	,	3427	},
	{	100	,	3425	},
	{	100	,	3422	},
	{	100	,	3420	},
	{	100	,	3418	},
	{	100	,	3417	},
	{	100	,	3400	}
};      
         
// T1 0C 
BATTERY_PROFILE_STRUC battery_profile_t1[] =
{              
	{	0	,	4171	},
	{	1	,	4147	},
	{	3	,	4129	},
	{	4	,	4114	},
	{	6	,	4100	},
	{	7	,	4089	},
	{	9	,	4079	},
	{	10	,	4069	},
	{	12	,	4053	},
	{	13	,	4037	},
	{	15	,	4021	},
	{	16	,	4007	},
	{	18	,	3997	},
	{	19	,	3988	},
	{	21	,	3980	},
	{	22	,	3972	},
	{	24	,	3965	},
	{	25	,	3957	},
	{	26	,	3948	},
	{	28	,	3939	},
	{	29	,	3930	},
	{	31	,	3921	},
	{	32	,	3910	},
	{	34	,	3899	},
	{	35	,	3887	},
	{	37	,	3876	},
	{	38	,	3866	},
	{	40	,	3857	},
	{	41	,	3849	},
	{	43	,	3842	},
	{	44	,	3836	},
	{	46	,	3829	},
	{	47	,	3824	},
	{	49	,	3820	},
	{	50	,	3815	},
	{	52	,	3811	},
	{	53	,	3806	},
	{	54	,	3803	},
	{	56	,	3800	},
	{	57	,	3796	},
	{	59	,	3793	},
	{	60	,	3790	},
	{	62	,	3788	},
	{	63	,	3785	},
	{	65	,	3782	},
	{	66	,	3780	},
	{	68	,	3778	},
	{	69	,	3775	},
	{	71	,	3773	},
	{	72	,	3769	},
	{	74	,	3766	},
	{	75	,	3762	},
	{	76	,	3758	},
	{	78	,	3754	},
	{	79	,	3749	},
	{	81	,	3743	},
	{	82	,	3737	},
	{	84	,	3731	},
	{	85	,	3724	},
	{	87	,	3716	},
	{	88	,	3707	},
	{	90	,	3702	},
	{	91	,	3699	},
	{	93	,	3696	},
	{	94	,	3689	},
	{	96	,	3660	},
	{	97	,	3604	},
	{	99	,	3525	},
	{	100	,	3398	},
	{	100	,	3348	},
	{	101	,	3330	},
	{	101	,	3321	},
	{	101	,	3315	},
	{	101	,	3313	},
	{	101	,	3313	}
};             


// T2 25C
BATTERY_PROFILE_STRUC battery_profile_t2[] =
{              
	{	0	,	4176	},
	{	1	,	4162	},
	{	3	,	4155	},
	{	4	,	4143	},
	{	6	,	4129	},
	{	7	,	4116	},
	{	8	,	4103	},
	{	10	,	4090	},
	{	11	,	4079	},
	{	13	,	4068	},
	{	14	,	4056	},
	{	16	,	4044	},
	{	17	,	4031	},
	{	18	,	4020	},
	{	20	,	4009	},
	{	21	,	3999	},
	{	23	,	3990	},
	{	24	,	3980	},
	{	25	,	3971	},
	{	27	,	3962	},
	{	28	,	3954	},
	{	30	,	3945	},
	{	31	,	3936	},
	{	33	,	3927	},
	{	34	,	3919	},
	{	35	,	3909	},
	{	37	,	3899	},
	{	38	,	3887	},
	{	40	,	3874	},
	{	41	,	3863	},
	{	42	,	3854	},
	{	44	,	3846	},
	{	45	,	3839	},
	{	47	,	3833	},
	{	48	,	3827	},
	{	50	,	3822	},
	{	51	,	3817	},
	{	52	,	3813	},
	{	54	,	3808	},
	{	55	,	3804	},
	{	57	,	3800	},
	{	58	,	3797	},
	{	60	,	3794	},
	{	61	,	3790	},
	{	62	,	3787	},
	{	64	,	3785	},
	{	65	,	3782	},
	{	67	,	3779	},
	{	68	,	3776	},
	{	69	,	3773	},
	{	71	,	3768	},
	{	72	,	3762	},
	{	74	,	3757	},
	{	75	,	3751	},
	{	77	,	3746	},
	{	78	,	3742	},
	{	79	,	3738	},
	{	81	,	3733	},
	{	82	,	3728	},
	{	84	,	3722	},
	{	85	,	3715	},
	{	86	,	3709	},
	{	88	,	3700	},
	{	89	,	3691	},
	{	91	,	3688	},
	{	92	,	3686	},
	{	93	,	3683	},
	{	95	,	3672	},
	{	96	,	3633	},
	{	98	,	3572	},
	{	99	,	3487	},
	{	101	,	3347	},
	{	101	,	3274	},
	{	101	,	3261	},
	{	101	,	3261	}
};             


// T3 50C
BATTERY_PROFILE_STRUC battery_profile_t3[] =
{              
	{	0	,	4192	},
	{	1	,	4175	},
	{	3	,	4161	},
	{	4	,	4147	},
	{	6	,	4132	},
	{	7	,	4119	},
	{	8	,	4106	},
	{	10	,	4094	},
	{	11	,	4081	},
	{	13	,	4069	},
	{	14	,	4057	},
	{	16	,	4046	},
	{	17	,	4034	},
	{	18	,	4023	},
	{	20	,	4013	},
	{	21	,	4002	},
	{	23	,	3992	},
	{	24	,	3982	},
	{	25	,	3973	},
	{	27	,	3963	},
	{	28	,	3954	},
	{	30	,	3945	},
	{	31	,	3936	},
	{	32	,	3927	},
	{	34	,	3919	},
	{	35	,	3910	},
	{	37	,	3901	},
	{	38	,	3890	},
	{	40	,	3878	},
	{	41	,	3865	},
	{	42	,	3855	},
	{	44	,	3847	},
	{	45	,	3840	},
	{	47	,	3833	},
	{	48	,	3828	},
	{	49	,	3821	},
	{	51	,	3817	},
	{	52	,	3812	},
	{	54	,	3807	},
	{	55	,	3803	},
	{	56	,	3799	},
	{	58	,	3795	},
	{	59	,	3791	},
	{	61	,	3788	},
	{	62	,	3785	},
	{	63	,	3782	},
	{	65	,	3778	},
	{	66	,	3775	},
	{	68	,	3773	},
	{	69	,	3767	},
	{	71	,	3758	},
	{	72	,	3749	},
	{	73	,	3744	},
	{	75	,	3738	},
	{	76	,	3733	},
	{	78	,	3728	},
	{	79	,	3724	},
	{	80	,	3719	},
	{	82	,	3714	},
	{	83	,	3708	},
	{	85	,	3701	},
	{	86	,	3694	},
	{	87	,	3687	},
	{	89	,	3678	},
	{	90	,	3674	},
	{	92	,	3672	},
	{	93	,	3668	},
	{	95	,	3659	},
	{	96	,	3628	},
	{	97	,	3574	},
	{	99	,	3500	},
	{	100	,	3387	},
	{	101	,	3269	},
	{	101	,	3256	},
	{	101	,	3256	}
};             
           

// battery profile for actual temperature. The size should be the same as T1, T2 and T3
BATTERY_PROFILE_STRUC battery_profile_temperature[] =
{
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },          
       {0  , 0 },
       {0  , 0 },
       {0  , 0 },
       {0  , 0 },
       {0  , 0 },
       {0  , 0 },
       {0  , 0 },
       {0  , 0 },
       {0  , 0 },
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }, 
	{0  , 0 },	    
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 },   
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }  
};      

/////////////////////////////////////////////////////////////////////
// <Rbat, Battery_Voltage> Table
/////////////////////////////////////////////////////////////////////
typedef struct _R_PROFILE_STRUC
{
    kal_int32 resistance; // Ohm
    kal_int32 voltage;
} R_PROFILE_STRUC, *R_PROFILE_STRUC_P;

// T0 -10C
R_PROFILE_STRUC r_profile_t0[] =
{              
	{	288	,	4177	},
	{	288	,	4162	},
	{	290	,	4145	},
	{	300	,	4118	},
	{	338	,	4091	},
	{	363	,	4071	},
	{	385	,	4052	},
	{	415	,	4032	},
	{	445	,	4012	},
	{	473	,	3996	},
	{	485	,	3986	},
	{	493	,	3978	},
	{	495	,	3969	},
	{	498	,	3961	},
	{	500	,	3953	},
	{	508	,	3945	},
	{	508	,	3936	},
	{	508	,	3926	},
	{	505	,	3916	},
	{	500	,	3904	},
	{	498	,	3894	},
	{	490	,	3882	},
	{	488	,	3872	},
	{	483	,	3862	},
	{	483	,	3854	},
	{	485	,	3847	},
	{	480	,	3839	},
	{	483	,	3833	},
	{	483	,	3827	},
	{	488	,	3823	},
	{	488	,	3817	},
	{	493	,	3813	},
	{	493	,	3808	},
	{	495	,	3804	},
	{	498	,	3801	},
	{	500	,	3797	},
	{	503	,	3794	},
	{	505	,	3791	},
	{	508	,	3788	},
	{	505	,	3784	},
	{	510	,	3782	},
	{	513	,	3780	},
	{	515	,	3778	},
	{	518	,	3776	},
	{	515	,	3773	},
	{	518	,	3770	},
	{	515	,	3766	},
	{	518	,	3763	},
	{	520	,	3759	},
	{	520	,	3754	},
	{	520	,	3749	},
	{	520	,	3744	},
	{	523	,	3738	},
	{	523	,	3731	},
	{	523	,	3724	},
	{	525	,	3717	},
	{	530	,	3711	},
	{	545	,	3707	},
	{	565	,	3702	},
	{	590	,	3695	},
	{	610	,	3678	},
	{	625	,	3646	},
	{	645	,	3595	},
	{	700	,	3521	},
	{	685	,	3474	},
	{	633	,	3453	},
	{	605	,	3441	},
	{	583	,	3433	},
	{	573	,	3427	},
	{	568	,	3425	},
	{	560	,	3422	},
	{	553	,	3420	},
	{	545	,	3418	},
	{	543	,	3417	},
	{	505	,	3400	}
};             
              


// T1 0C
R_PROFILE_STRUC r_profile_t1[] =
{              
	{	235	,	4171	},
	{	235	,	4147	},
	{	245	,	4129	},
	{	253	,	4114	},
	{	255	,	4100	},
	{	263	,	4089	},
	{	268	,	4079	},
	{	275	,	4069	},
	{	270	,	4053	},
	{	278	,	4037	},
	{	280	,	4021	},
	{	280	,	4007	},
	{	285	,	3997	},
	{	285	,	3988	},
	{	288	,	3980	},
	{	293	,	3972	},
	{	298	,	3965	},
	{	300	,	3957	},
	{	298	,	3948	},
	{	300	,	3939	},
	{	303	,	3930	},
	{	303	,	3921	},
	{	298	,	3910	},
	{	293	,	3899	},
	{	285	,	3887	},
	{	280	,	3876	},
	{	275	,	3866	},
	{	273	,	3857	},
	{	270	,	3849	},
	{	270	,	3842	},
	{	270	,	3836	},
	{	268	,	3829	},
	{	270	,	3824	},
	{	273	,	3820	},
	{	273	,	3815	},
	{	275	,	3811	},
	{	275	,	3806	},
	{	278	,	3803	},
	{	283	,	3800	},
	{	283	,	3796	},
	{	285	,	3793	},
	{	288	,	3790	},
	{	290	,	3788	},
	{	293	,	3785	},
	{	290	,	3782	},
	{	293	,	3780	},
	{	295	,	3778	},
	{	290	,	3775	},
	{	293	,	3773	},
	{	290	,	3769	},
	{	293	,	3766	},
	{	290	,	3762	},
	{	288	,	3758	},
	{	293	,	3754	},
	{	293	,	3749	},
	{	293	,	3743	},
	{	293	,	3737	},
	{	295	,	3731	},
	{	295	,	3724	},
	{	295	,	3716	},
	{	290	,	3707	},
	{	295	,	3702	},
	{	303	,	3699	},
	{	320	,	3696	},
	{	343	,	3689	},
	{	343	,	3660	},
	{	343	,	3604	},
	{	365	,	3525	},
	{	408	,	3398	},
	{	375	,	3348	},
	{	325	,	3330	},
	{	305	,	3321	},
	{	293	,	3315	},
	{	283	,	3313	},
	{	288	,	3313	}
}; 


// T2 25C
R_PROFILE_STRUC r_profile_t2[] =
{              
	{	140	,	4188	},
	{	140	,	4172	},
	{	140	,	4157	},
	{	143	,	4143	},
	{	140	,	4129	},
	{	140	,	4116	},
	{	143	,	4103	},
	{	143	,	4090	},
	{	145	,	4079	},
	{	145	,	4068	},
	{	145	,	4056	},
	{	148	,	4044	},
	{	148	,	4031	},
	{	150	,	4020	},
	{	153	,	4009	},
	{	153	,	3999	},
	{	158	,	3990	},
	{	155	,	3980	},
	{	160	,	3971	},
	{	160	,	3962	},
	{	168	,	3954	},
	{	168	,	3945	},
	{	170	,	3936	},
	{	170	,	3927	},
	{	173	,	3919	},
	{	173	,	3909	},
	{	170	,	3899	},
	{	165	,	3887	},
	{	158	,	3874	},
	{	150	,	3863	},
	{	148	,	3854	},
	{	148	,	3846	},
	{	145	,	3839	},
	{	145	,	3833	},
	{	145	,	3827	},
	{	145	,	3822	},
	{	145	,	3817	},
	{	145	,	3813	},
	{	148	,	3808	},
	{	150	,	3804	},
	{	148	,	3800	},
	{	150	,	3797	},
	{	153	,	3794	},
	{	153	,	3790	},
	{	153	,	3787	},
	{	158	,	3785	},
	{	158	,	3782	},
	{	158	,	3779	},
	{	158	,	3776	},
	{	158	,	3773	},
	{	155	,	3768	},
	{	150	,	3762	},
	{	150	,	3757	},
	{	148	,	3751	},
	{	148	,	3746	},
	{	148	,	3742	},
	{	148	,	3738	},
	{	150	,	3733	},
	{	150	,	3728	},
	{	153	,	3722	},
	{	150	,	3715	},
	{	153	,	3709	},
	{	153	,	3700	},
	{	148	,	3691	},
	{	153	,	3688	},
	{	155	,	3686	},
	{	163	,	3683	},
	{	165	,	3672	},
	{	160	,	3633	},
	{	165	,	3572	},
	{	173	,	3487	},
	{	195	,	3347	},
	{	188	,	3274	},
	{	153	,	3261	},
	{	155	,	3261	}
};




// T3 50C
R_PROFILE_STRUC r_profile_t3[] =
{              
	{	115	,	4192	},
	{	115	,	4175	},
	{	118	,	4161	},
	{	120	,	4147	},
	{	115	,	4132	},
	{	118	,	4119	},
	{	118	,	4106	},
	{	120	,	4094	},
	{	120	,	4081	},
	{	118	,	4069	},
	{	118	,	4057	},
	{	123	,	4046	},
	{	120	,	4034	},
	{	123	,	4023	},
	{	120	,	4013	},
	{	123	,	4002	},
	{	125	,	3992	},
	{	125	,	3982	},
	{	128	,	3973	},
	{	130	,	3963	},
	{	130	,	3954	},
	{	130	,	3945	},
	{	133	,	3936	},
	{	135	,	3927	},
	{	140	,	3919	},
	{	140	,	3910	},
	{	140	,	3901	},
	{	135	,	3890	},
	{	133	,	3878	},
	{	125	,	3865	},
	{	123	,	3855	},
	{	123	,	3847	},
	{	125	,	3840	},
	{	123	,	3833	},
	{	125	,	3828	},
	{	120	,	3821	},
	{	123	,	3817	},
	{	125	,	3812	},
	{	125	,	3807	},
	{	125	,	3803	},
	{	125	,	3799	},
	{	125	,	3795	},
	{	125	,	3791	},
	{	128	,	3788	},
	{	130	,	3785	},
	{	130	,	3782	},
	{	133	,	3778	},
	{	135	,	3775	},
	{	140	,	3773	},
	{	133	,	3767	},
	{	125	,	3758	},
	{	123	,	3749	},
	{	125	,	3744	},
	{	125	,	3738	},
	{	128	,	3733	},
	{	125	,	3728	},
	{	128	,	3724	},
	{	125	,	3719	},
	{	128	,	3714	},
	{	128	,	3708	},
	{	128	,	3701	},
	{	125	,	3694	},
	{	128	,	3687	},
	{	125	,	3678	},
	{	125	,	3674	},
	{	128	,	3672	},
	{	133	,	3668	},
	{	135	,	3659	},
	{	135	,	3628	},
	{	138	,	3574	},
	{	143	,	3500	},
	{	155	,	3387	},
	{	175	,	3269	},
	{	140	,	3256	},
	{	143	,	3256	}
};

// r-table profile for actual temperature. The size should be the same as T1, T2 and T3
R_PROFILE_STRUC r_profile_temperature[] =
{
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },          
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }, 
	{0  , 0 },	    
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 },   
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }  
};   


int fgauge_get_saddles(void);
BATTERY_PROFILE_STRUC_P fgauge_get_profile(kal_uint32 temperature);

int fgauge_get_saddles_r_table(void);
R_PROFILE_STRUC_P fgauge_get_profile_r_table(kal_uint32 temperature);

//#define CONFIG_POWER_VERIFY
