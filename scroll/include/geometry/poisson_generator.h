#ifndef POISSON_GENERATOR_H
#define POISSON_GENERATOR_H

#include <vector>
#include <stdint.h>
#include <time.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Point_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2											Point_2;

namespace scl
{
	namespace PoissonGenerator
	{
		struct sPoint
		{
			sPoint() : x(0), y(0), m_Valid(false) {}
			sPoint(float X, float Y) : x(X), y(Y), m_Valid(true) {}
			float x;
			float y;
			bool m_Valid;

			bool isInRectangle() const
			{
				return x >= 0 && y >= 0 && x <= 1 && y <= 1;
			}

			bool isInCircle() const
			{
				float fx = x - 0.5f;
				float fy = y - 0.5f;
				return (fx*fx + fy * fy) <= 0.25f;
			}
		};

		struct sGridPoint
		{
			sGridPoint(int X, int Y) : x(X), y(Y) {}
			int x;
			int y;
		};

		inline float getDistance(const sPoint& P1, const sPoint& P2)
		{
			return sqrt((P1.x - P2.x) * (P1.x - P2.x) + (P1.y - P2.y) * (P1.y - P2.y));
		}

		inline sGridPoint imageToGrid(const sPoint& P, float CellSize)
		{
			return sGridPoint((int)(P.x / CellSize), (int)(P.y / CellSize));
		}

		struct sGrid
		{
			sGrid(int W, int H, float CellSize) : m_W(W), m_H(H), m_CellSize(CellSize)
			{
				m_Grid.resize(m_H);

				for (auto i = m_Grid.begin(); i != m_Grid.end(); i++) { i->resize(m_W); }
			}
			void insert(const sPoint& P)
			{
				sGridPoint G = imageToGrid(P, m_CellSize);
				m_Grid[G.x][G.y] = P;
			}
			bool isInNeighbourhood(sPoint Point, float MinDist, float CellSize)
			{
				sGridPoint G = imageToGrid(Point, CellSize);

				//number of adjucent cells to look for neighbour points
				const int D = 5;

				//scan the neighbourhood of the point in the grid
				for (int i = G.x - D; i < G.x + D; i++)
				{
					for (int j = G.y - D; j < G.y + D; j++)
					{
						if (i >= 0 && i < m_W && j >= 0 && j < m_H)
						{
							sPoint P = m_Grid[i][j];

							if (P.m_Valid && getDistance(P, Point) < MinDist) { return true; }
						}
					}
				}


				return false;
			}

		private:
			int m_W;
			int m_H;
			float m_CellSize;

			std::vector< std::vector<sPoint> > m_Grid;
		};

		template <typename PRNG>
		inline sPoint popRandom(std::vector<sPoint>& Points, PRNG& Generator)
		{
			const int Idx = Generator.randomInt(static_cast<int>(Points.size()) - 1);
			const sPoint P = Points[Idx];
			Points.erase(Points.begin() + Idx);
			return P;
		}

		template <typename PRNG>
		inline sPoint generateRandomPointAround(const sPoint& P, float MinDist, PRNG& Generator)
		{
			//start with non-uniform distribution
			float R1 = Generator.randomFloat();
			float R2 = Generator.randomFloat();

			//radius should be between MinDist and 2 * MinDist
			float Radius = MinDist * (R1 + 1.0f);

			//random angle
			float Angle = 2 * 3.141592653589f * R2;

			//the new point is generated around the point (x, y)
			float X = P.x + Radius * cos(Angle);
			float Y = P.y + Radius * sin(Angle);

			return sPoint(X, Y);
		}

		/**
		Return a vector of generated points

		NewPointsCount - refer to bridson-siggraph07-poissondisk.pdf for details (the value 'k')
		Circle  - 'true' to fill a circle, 'false' to fill a rectangle
		MinDist - minimal distance estimator, use negative value for default
		**/
		template<typename PRNG = scl::DefaultPRNG>
		inline std::vector<Point_2> generatePoissonPoints(size_t NumPoints, PRNG& Generator, int NewPointsCount = 30, bool Circle = true, float MinDist = -1.0f)
		{
			if (MinDist < 0.0f)
			{
				MinDist = sqrt(float(NumPoints)) / float(NumPoints);
			}

			std::vector<sPoint> SamplePoints;
			std::vector<sPoint> ProcessList;

			//create the grid
			float CellSize = MinDist / sqrt(2.0f);

			int GridW = (int)ceil(1.0f / CellSize);
			int GridH = (int)ceil(1.0f / CellSize);

			sGrid Grid(GridW, GridH, CellSize);

			sPoint FirstPoint;
			do {
				FirstPoint = sPoint(Generator.randomFloat(), Generator.randomFloat());
			} while (!(Circle ? FirstPoint.isInCircle() : FirstPoint.isInRectangle()));

			// update containers
			ProcessList.push_back(FirstPoint);
			SamplePoints.push_back(FirstPoint);
			Grid.insert(FirstPoint);

			// generate new points for each point in the queue
			while (!ProcessList.empty() && SamplePoints.size() < NumPoints)
			{
				sPoint Point = popRandom<PRNG>(ProcessList, Generator);

				for (int i = 0; i < NewPointsCount; i++)
				{
					sPoint NewPoint = generateRandomPointAround(Point, MinDist, Generator);

					bool Fits = Circle ? NewPoint.isInCircle() : NewPoint.isInRectangle();

					if (Fits && !Grid.isInNeighbourhood(NewPoint, MinDist, CellSize))
					{
						ProcessList.push_back(NewPoint);
						SamplePoints.push_back(NewPoint);
						Grid.insert(NewPoint);
						continue;
					}
				}
			}

			std::vector<Point_2> points;
			for (sPoint el : SamplePoints)
			{
				points.push_back(Point_2(el.x, el.y));
			}

			return points;
		}
	}
}

#endif