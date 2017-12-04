#!wolframscript -script

(* setup *)

SetDirectory["mathematica"];
Needs["CGR`"]; Needs["CPUDistances`"];

numSeqs = 100;

Print["Random generation (" <> ToString[numSeqs] <> " seqs): " <> ToString@First@AbsoluteTiming[
	seqs = Map[StringJoin, RandomChoice[{"A", "C", "T", "G"}, {numSeqs, 10^7}]];
]];

(* serial tests *)

Print[];
Print["Serial tests:"];
canarySerial = 0;

Print["CGR: " <> ToString@First@AbsoluteTiming[
	squareCGRs = Map[N@FCGR[#, {"A", "C", "G", "T"}, 9]&, seqs];
]];
flatCGRs = Flatten /@ squareCGRs;

Print["Descriptors: " <> ToString@First@AbsoluteTiming[
	canarySerial += Total[Map[(
		(N@Descriptor[#, {8}, {8}, {0, 1, 2, 5, 20, Infinity}])[[1]]
	)&, squareCGRs]];
]];

Print["Euclidean: " <> ToString@First@AbsoluteTiming[
	canarySerial += Total[Outer[EuclideanDistance, flatCGRs, flatCGRs, 1], Infinity];
]];

Print["Manhattan: " <> ToString@First@AbsoluteTiming[
	canarySerial += Total[Outer[ManhattanDistance, flatCGRs, flatCGRs, 1], Infinity];
]];

Print["Info-dist: " <> ToString@First@AbsoluteTiming[
	canarySerial += Total[Outer[N @* ApproxInfoDist, flatCGRs, flatCGRs, 1], Infinity];
]];

Print["Cosine: " <> ToString@First@AbsoluteTiming[
	canarySerial += Total[Outer[N @* CosineDistance, flatCGRs, flatCGRs, 1], Infinity];
]];

Print["Pearson: " <> ToString@First@AbsoluteTiming[
	canarySerial += Total[Outer[N @* CorrelationDistance, flatCGRs, flatCGRs, 1], Infinity];
]];

Print["SSIM: " <> ToString@First@AbsoluteTiming[
	canarySerial += Total[Outer[N @* SSIM, squareCGRs, squareCGRs, 1], Infinity];
]];

(* parallel tests *)

numThreads = $ProcessorCount * 2;

CloseKernels[];
LaunchKernels[numThreads];
ParallelNeeds["CGR`"]; ParallelNeeds["CPUDistances`"];
DistributeDefinitions[seqs];

Print[];
Print["Parallel tests (" <> ToString[numThreads] <> " threads):"];
canaryParallel = 0;

jobs = Table[
	ParallelSubmit[{i},
		N@FCGR[seqs[[i]], {"A", "C", "G", "T"}, 9]
	]
, {i, 1, numSeqs}];
Print["CGR: " <> ToString@First@AbsoluteTiming[
	squareCGRs = WaitAll[jobs];
]];
flatCGRs = Flatten /@ squareCGRs;
DistributeDefinitions[flatCGRs, squareCGRs];

jobs = Table[
	ParallelSubmit[{i},
		N@Descriptor[squareCGRs[[i]], {8}, {8}, {0, 1, 2, 5, 20, Infinity}][[1]]
	]
, {i, 1, numSeqs}];
Print["Descriptors: " <> ToString@First@AbsoluteTiming[
	canaryParallel += Total@WaitAll[jobs];
]];

jobs = Table[
	ParallelSubmit[{i},
		Total@Map[EuclideanDistance[#, flatCGRs[[i]]]&, flatCGRs]
	]
, {i, 1, numSeqs}];
Print["Euclidean: " <> ToString@First@AbsoluteTiming[
	canaryParallel += Total@WaitAll[jobs];
]];

jobs = Table[
	ParallelSubmit[{i},
		Total@Map[ManhattanDistance[#, flatCGRs[[i]]]&, flatCGRs]
	]
, {i, 1, numSeqs}];
Print["Manhattan: " <> ToString@First@AbsoluteTiming[
	canaryParallel += Total@WaitAll[jobs];
]];

jobs = Table[
	ParallelSubmit[{i},
		Total@Map[N@ApproxInfoDist[#, flatCGRs[[i]]]&, flatCGRs]
	]
, {i, 1, numSeqs}];
Print["Info-dist: " <> ToString@First@AbsoluteTiming[
	canaryParallel += Total@WaitAll[jobs];
]];

jobs = Table[
	ParallelSubmit[{i},
		Total@Map[N@CosineDistance[#, flatCGRs[[i]]]&, flatCGRs]
	]
, {i, 1, numSeqs}];
Print["Cosine: " <> ToString@First@AbsoluteTiming[
	canaryParallel += Total@WaitAll[jobs];
]];

jobs = Table[
	ParallelSubmit[{i},
		Total@Map[N@CorrelationDistance[#, flatCGRs[[i]]]&, flatCGRs]
	]
, {i, 1, numSeqs}];
Print["Pearson: " <> ToString@First@AbsoluteTiming[
	canaryParallel += Total@WaitAll[jobs];
]];

jobs = Table[
	ParallelSubmit[{i},
		Total@Map[N@SSIM[#, squareCGRs[[i]]]&, squareCGRs]
	]
, {i, 1, numSeqs}];
Print["SSIM: " <> ToString@First@AbsoluteTiming[
	canaryParallel += Total@WaitAll[jobs];
]];

(* canary *)

Print[];
Print["If these two numbers are not the same, something went wrong: " <>
	  ToString[canarySerial, InputForm] <>  ", " <> ToString[canaryParallel, InputForm]];
Print[If[Abs[canarySerial - canaryParallel] < 1*^-5, "All OK", "Something went wrong"] <>
	  ", delta = " <> ToString[Abs[canarySerial - canaryParallel], InputForm]];
