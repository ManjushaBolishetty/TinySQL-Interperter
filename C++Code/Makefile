all: Main_DB

Main_DB: FileController.o ConstructQueryPlan.o Analyzer.o Lexer.o SqlProcessor.o LogicalPlan.o Processing.o GeneratorLQP.o LqpOptimizer.o Comparison.o Expressions.o TupleOperations.o TupleProcessing.o find.o MyToken.o TableProcessing.o StorageManager.o
	g++ -o Main_DB FileController.o ConstructQueryPlan.o Analyzer.o Lexer.o SqlProcessor.o LogicalPlan.o Processing.o GeneratorLQP.o LqpOptimizer.o Comparison.o Expressions.o TupleOperations.o TupleProcessing.o find.o MyToken.o TableProcessing.o StorageManager.o Main_DB.cpp

FileController.o: FileController.h
	g++ -c -std=c++11 FileController.cpp

ConstructQueryPlan.o: ConstructQueryPlan.h Block.h Config.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h SendRelation.h Processing.h TupleOperations.h TableProcessing.h TupleProcessing.h
	g++ -c -std=c++11 ConstructQueryPlan.cpp

Analyzer.o: Analyzer.h Block.h Config.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h DB_config.h Lexer.h SqlProcessor.h LogicalPlan.h Processing.h GeneratorLQP.h LqpOptimizer.h
	g++ -c -std=c++11 Analyzer.cpp

Lexer.o: Lexer.h MyToken.h MyKeywords.h
	g++ -c -std=c++11 Lexer.cpp

SqlProcessor.o: SqlProcessor.h Block.h Config.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h LogicalPlan.h
	g++ -c -std=c++11 SqlProcessor.cpp

LogicalPlan.o: LogicalPlan.h Processing.h TupleOperations.h ConstructQueryPlan.h TableProcessing.h SendRelation.h
	g++ -c -std=c++11 LogicalPlan.cpp

Processing.o: Processing.h TupleOperations.h find.h DB_config.h
	g++ -c -std=c++11 Processing.cpp

GeneratorLQP.o: GeneratorLQP.h LogicalPlan.h find.h Processing.h
	g++ -c -std=c++11 GeneratorLQP.cpp

LqpOptimizer.o: LqpOptimizer.h LogicalPlan.h
	g++ -c -std=c++11 LqpOptimizer.cpp

Comparison.o: Comparison.h Expressions.h
	g++ -c -std=c++11 Comparison.cpp

Expressions.o: Expressions.h TupleOperations.h Block.h Config.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h
	g++ -c -std=c++11 Expressions.cpp

TupleOperations.o: TupleOperations.h Block.h Config.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h Processing.h DB_config.h SendRelation.h TableProcessing.h TupleProcessing.h
	g++ -c -std=c++11 TupleOperations.cpp

TupleProcessing.o: TupleProcessing.h Block.h Config.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h DB_config.h
	g++ -c -std=c++11 TupleProcessing.cpp

find.o: find.h Comparison.h
	g++ -c -std=c++11 find.cpp

MyToken.o: MyToken.h
	g++ -c -std=c++11 MyToken.cpp

TableProcessing.o: TableProcessing.h
	g++ -c -std=c++11 TableProcessing.cpp

StorageManager.o: Block.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h Config.h
	g++ -c -std=c++11 StorageManager.cpp

clean:
	rm *.o Main_DB

#TestStorageManager: StorageManager.o
#	g++ -o TestStorageManager StorageManager.o TestStorageManager.cpp
#
#StorageManager.o: Block.h Disk.h Field.h MainMemory.h Relation.h Schema.h SchemaManager.h Tuple.h Config.h
#	g++ -c StorageManager.cpp
#
#clean:
#	rm *.o TestStorageManager
