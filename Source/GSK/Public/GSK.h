#pragma once
#include <StandardTypes/StandardTypes.h>

namespace GSK
{
	enum Property
	{
		Age
	};

	struct Entity
	{
		UnorderedMap<String, double> properties;
		bool alive = true;
	};

	enum CompareOp
	{
		Greater,
		Less,
		Equal,
		NotEqual,
		GreaterEqual,
		LessEqual
	};

	struct Condition
	{
		Property property;
		CompareOp op;
		double value;
	};

	enum EffectOp
	{
		Add,
		Multiply,
		Set
	};

	struct Effect
	{
		Property property;
		EffectOp op;
		double value;
	};

	struct Rule
	{
		double interval;
		double accumulator = 0.0;

		Condition condition;
		Effect effect;
	};

	struct World
	{
		double elapsedTime = 0.0;
		Vector<Entity> entities;
	};

	/**
	* GSK - Game Simulation Kernal.
	*/
	class GSK
	{
	public:
		GSK();
		~GSK();

		void Step();

	private:

	};
}