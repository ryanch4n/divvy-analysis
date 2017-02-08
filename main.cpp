/*main.cpp*/

//
// Divvy analysis: station and ride analysis.
//
// << your name >>
// << your development environment >>
// U. of Illinois, Chicago
// CS341, Spring 2017
// Project #02
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <cmath>
 
using namespace std;

class Station
{
private:
	int ID, Capacity, TotalRides, MorningRides, AfternoonRides, NightRides;
	string Name;
	double Lat;
	double Lon;

public:
	//
	// constructor:
	//
	Station(int id, string name, double lat, double lon, int cap)
		: ID(id), Name(name), Lat(lat), Lon(lon), Capacity(cap)
	{
		TotalRides = 0;
		MorningRides = 0;
		AfternoonRides = 0;
		NightRides = 0;
	}

	//
	// getters/setters:
	//
	int getStationID() const
	{
		return ID;
	}

	string getStationName() const
	{
		return Name;
	}

	double getLat() const
	{
		return Lat;
	}

	double getLon() const
	{
		return Lon;
	}

	int getCapacity() const
	{
		return Capacity;
	}

	int getTotalRides() const
	{
		return TotalRides;
	}

	void increaseTotalRides()
	{
		TotalRides++;
	}

	int getMorningRides() const
	{
		return MorningRides;
	}

	void increaseMorningRides()
	{
		MorningRides++;
	}

	int getAfternoonRides() const
	{
		return AfternoonRides;
	}

	void increaseAfternoonRides()
	{
		AfternoonRides++;
	}

	int getNightRides() const
	{
		return NightRides;
	}

	void increaseNightRides()
	{
		NightRides++;
	}
};

//
// FindStation: searches the vector for the station with matching
// station id; returns iterator if found, otherwise end() is returned.
//
auto FindStation(vector<Station>& stations, int stationID)
{
	auto iter = std::find_if(stations.begin(), stations.end(),
		[=](const Station& s)
		{
			if (s.getStationID() == stationID)
				return true;
			else
				return false;
		}
	);

	return iter;
}

//
// ProcessStations: inputs the stations from the given file, and stores
// the data in a vector of Station objects.  The vector is returned.
//
vector<Station> ProcessStations(string filename, int& numStations)
{
	ifstream file(filename);
	vector<Station> stations;
	string line;

	getline(file, line); // discard first line (column headers):

	while (getline(file, line))
	{
		stringstream ss(line);

		// format: StationID,StationName,Latitude,Longitude,Capacity,OnlineDate
		string stationID, stationName, lat, lon, capacity;

		getline(ss, stationID, ',');
		getline(ss, stationName, ',');
		getline(ss, lat, ',');
		getline(ss, lon, ',');
		getline(ss, capacity, ',');
		// ignore OnlineDate, we don't need it:

		Station s(stoi(stationID), stationName, stod(lat), stod(lon), stoi(capacity));

		stations.push_back(s);

		numStations++;
	}

	//
	// done, return input:
	//
	return stations;
}

//
// ProcessRides: inputs rides from the given file, and for each
// review updates the corresponding station: # of rides and the
// counts of time of rides.  Returns the # of rides processed.
//
long long ProcessRides(string filename, vector<Station>& stations, vector<int>& hours)
{
	ifstream file(filename);
	long long numRides = 0;
	string line;

	getline(file, line); // discard first line (column headers):

	while (getline(file, line))
	{
		numRides++;

		stringstream ss(line);

		// format: trip_id,starttime,stoptime,bikeid,tripduration,from_station_id,from_station_name,to_station_id,to_station_name,usertype,gender,birthyear
		string startTime, fromID, toID, ignoreVar;

		getline(ss, ignoreVar, ','); // ignore trip_id
		getline(ss, ignoreVar, ' '); //ignore upto date space
		getline(ss, startTime, ',');
		getline(ss, ignoreVar, ','); // ignore stoptime
		getline(ss, ignoreVar, ','); // ignore bikeid
		getline(ss, ignoreVar, ','); // ignore tripduration
		getline(ss, fromID, ',');
		getline(ss, ignoreVar, ','); // ignore from_station_name
		getline(ss, toID, ',');
		// ignore to_station_name, usertype, gender, birthyear		

		int hour = 0;

		if (startTime.at(1) == ':') // Single Digit Hour
			hour = stoi(startTime.substr(0, 1));
		else
			hour = stoi(startTime.substr(0, 2));

		// Update FROM station
		auto i = FindStation(stations, stoi(fromID));
		if (i != stations.end())
		{
			i->increaseTotalRides();
			hours[hour]++;
			
			if (6 <= hour && hour < 9)
				i->increaseMorningRides();
			else if (12 <= hour && hour < 13)
				i->increaseAfternoonRides();
			else if (15 <= hour && hour < 19)
				i->increaseNightRides();
		}

		//Update TO station
		i = FindStation(stations, stoi(toID));
		if (i != stations.end())
		{
			i->increaseTotalRides();
		}
	}

	return numRides;
}

void info(vector<Station>& stations, int searchID)
{
	auto i = FindStation(stations, searchID);
	if (i != stations.end())
	{
		cout << "Name:     '" << i->getStationName() << "'" << endl;
		cout << "Position: (" << i->getLat() << ", " << i->getLon() << ")" << endl;
		cout << "Capacity: " << i->getCapacity() << endl;
		cout << "Total rides to/from: " << i->getTotalRides() << endl;
		cout << "Num rides originating:" << endl;
		cout << "  6-9am:    " << i->getMorningRides() << endl;
		cout << "  noon-1pm: " << i->getAfternoonRides() << endl;
		cout << "  3-7pm:    " << i->getNightRides() << endl;

		return;
	}

	cout << "**Not found..." << endl;

}

void topN(vector<Station>& stations, int N)
{
	sort(stations.begin(),
		stations.end(),
		[](const Station& s1, const Station& s2)
		{
			if (s1.getTotalRides() > s2.getTotalRides())
				return true;
			else if (s1.getTotalRides() < s2.getTotalRides())
				return false;
			else // rides are equal, secondary sort by station name:
			{
				if (s1.getStationName() < s2.getStationName())
					return true;
				else
					return false;
			}
		}
	);

	if (N > (int)stations.size())
		N = (int)stations.size();
		
	for (int i = 0; i < N; i++)
	{
		cout << i + 1 << ". Station " << stations[i].getStationID() 
			<< ": " << stations[i].getTotalRides() << " rides @ '" 
			<< stations[i].getStationName() << "'" << endl;
	}
}


void hourly(vector<int>& hours)
{
	int index = 0;

	int count = accumulate(hours.begin(), hours.end(), 0,
		[&](int total, const int& i)
		{
			cout << index << ": " << i << endl;
			index++;
			return i + total;
		}
	);

	cout << "(" << count << ")" << endl;
}


//
// DistBetween2Points: returns the distance in miles between 2 points 
// (lat1, long1) and (lat2, long2).
// 
double distBetween2Points(double lat1, double long1, double lat2, double long2)
{
  //
  // Reference: http://www8.nau.edu/cvm/latlon_formula.html
  //
  constexpr double PI = 3.14159265;
  constexpr double earth_rad = 3963.1;  // statue miles:

  double lat1_rad = lat1 * PI / 180.0;
  double long1_rad = long1 * PI / 180.0;
  double lat2_rad = lat2 * PI / 180.0;
  double long2_rad = long2 * PI / 180.0;

  double dist = earth_rad * acos(
    (cos(lat1_rad)*cos(long1_rad)*cos(lat2_rad)*cos(long2_rad))
    +
    (cos(lat1_rad)*sin(long1_rad)*cos(lat2_rad)*sin(long2_rad))
    +
    (sin(lat1_rad)*sin(lat2_rad))
  );

  return dist;
}


void find(vector<Station> stations, double lat, double lon, double maxDist)
{
	multimap<double, string> M;
	double distance = 0.0;

	for (auto& s : stations)
	{
		distance = distBetween2Points(lat, lon, s.getLat(), s.getLon());

		if (distance <= maxDist)
		{
			M.insert(make_pair(distance, to_string(s.getStationID())));
		}
	}

	for (const auto& pair : M)
	{
		cout << "Station " << pair.second << ": " << pair.first << " miles" << endl;
	}
}


//
// getFileName: inputs a filename from the keyboard, make sure the file can be
// opened, and returns the filename if so.  If the file cannot be opened, an
// error message is output and the program is exited.
//
string getFileName()
{
  string filename;

  // input filename from the keyboard:
  getline(cin, filename);

  // make sure filename exists and can be opened:
  ifstream file(filename);
  if (!file.good())
  {
    cout << "**Error: cannot open file: " << filename << endl;
    std::terminate();
  }

  return filename;
}


int main()
{
  // setup output format:
  cout << std::fixed;
  cout << std::setprecision(4);

  // get filenames from the user/stdin:
  string StationsFileName = getFileName();
  string RidesFilename = getFileName();

  vector<int> hours (24, 0);

  // input & process station data:
  int numStations = 0;
  vector<Station> stations = ProcessStations(StationsFileName, numStations);
  cout << "**Num stations: " << numStations << endl;

  // process rides data:
  long long numRides = ProcessRides(RidesFilename, stations, hours);
  cout << "**Num rides:    " << numRides << endl;

  
  //
  // allow user to enter 0 or more commands, stop when 
  // the user inputs the command "exit":
  //
  string command, restOfLine;

  cin >> command;

  while (command != "exit")
  {
    if (command == "find")
    {
      //
      // find latitude longitude distance
      //
      // Example: find 41.86 -87.62 0.5
      //
      double latitude, longitude, distance;
      
      cin >> latitude;
      cin >> longitude;
      cin >> distance;


	  find(stations, latitude, longitude, distance);

    }
    else if (command == "info")
    {
      //
      // info stationID:
      //
      // Example: info 338
      //
      int stationID;

      cin >> stationID;

	  info(stations, stationID);

    }
    else if (command == "top")
    {
      //
      // top N:
      //
      // Example: top 10
      //
      int N;

      cin >> N;

      if (N < 1)  // invalid input?  Default to top 10:
        N = 10;

	  topN(stations, N);

    }
    else if (command == "hourly")
    {
      //
      // hourly:
      //

	  hourly(hours);


    }
    else
    {
      cout << "**Invalid, try again" << endl;
    }

    getline(cin, restOfLine);  // discard rest of line:

    //
    // input next command and repeat:
    cin >> command;
  }

  //
  // done:
  //
  return 0;
}
