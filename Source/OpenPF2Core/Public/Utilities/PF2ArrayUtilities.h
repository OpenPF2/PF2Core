// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

/**
 * Various utilities for functional programming with arrays and similar structures.
 */
namespace PF2ArrayUtilities
{
	/**
	 * Applies a transformation function to the values in an array, then returns the result of the transformation.
	 *
	 * The original array is not modified.
	 *
	 * @param Elements
	 *	The array of values to map.
	 * @param Callable
	 *	The transformation function/lambda invoked on each element in order to get the mapped value.
	 *
	 * @return
	 *	An array of the values that resulted from applying the transformation to every value of the source array.
	 */
	template <typename Out, typename In, typename Func>
	TArray<Out> Map(const TArray<In> Elements, const Func Callable)
	{
		TArray<Out> Result;

		for (const In& Element : Elements)
		{
			const Out MappingOutput = Callable(Element);

			Result.Add(MappingOutput);
		}

		return Result;
	}

	/**
	 * Collapses all of the values of an array to a single value by use of a transformation function.
	 *
	 * The transformation function receives a pair of values -- the "previous value" and the "current value". The
	 * transformation function is invoked once for each value in the original array. For the first value of the array,
	 * the "previous value" is the starting value supplied to this function, and the "current value" is the first value
	 * in the original array. For each subsequent call, the "previous value" is the result that was returned during the
	 * previous call of the transformation function.
	 *
	 * The original array is not modified.
	 *
	 * @param Elements
	 *	The array of values to reduce.
	 * @param StartingValue
	 *	The value passed-in to the Callable along side the first value of the array. If the array is empty, this will be
	 *	the value returned.
	 * @param Callable
	 *	The transformation function/lambda invoked to combine each element with the result of flattening/reducing the
	 *	previous element. This function is expected to take in the following two parameters and return a value that
	 *	matches the "Out" type:
	 *	  - PreviousValue: Which must match the "Out" type.
	 *	  - CurrentValue: Which must match the "In" type.
	 *
	 * @return
	 *	The result of reducing the values of the array.
	 */
	template <typename Out, typename In, typename Func>
	Out Reduce(const TArray<In> Elements, const Out StartingValue, const Func Callable)
	{
		Out PreviousValue = StartingValue;

		for (const In& CurrentValue : Elements)
		{
			PreviousValue = Callable(PreviousValue, CurrentValue);
		}

		return PreviousValue;
	}

	/**
	 * Filters out null values from an array.
	 *
	 * The original array is not modified.
	 *
	 * @param Elements
	 *	The array of values to filter.
	 *
	 * @return
	 *	A new array containing all the values of the original array that were not null.
	*/
	template <typename In>
	TArray<In> Filter(const TArray<In> Elements)
	{
		return Filter<In>(
			Elements,
			[](const In Element)
			{
				return Element != nullptr;
			}
		);
	}

	/**
	 * Filters the values of an array using a predicate function.
	 *
	 * The predicate receives each value of the array and is expected to return true if a value should be included, or
	 * false if it should be omitted.
	 *
	 * The original array is not modified.
	 *
	 * @param Elements
	 *	The array of values to filter.
	 * @param Callable
	 *	The predicate function/lambda invoked for each element of the array. This function is expected to take in a
	 *	parameter of type "In" and return a boolean.
	 *
	 * @return
	 *	A new array containing all the values of the original array for which the callable returned "true".
	 */
	template <typename In, typename Func>
	TArray<In> Filter(const TArray<In> Elements, const Func Callable)
	{
		return Elements.FilterByPredicate(Callable);
	}
}
