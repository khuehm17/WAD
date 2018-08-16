using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MockProjectInterface
{
    public class CWheelChair
    {
        public string id { get; set; }
        public string userName { get; set; }
        public string roomID { get; set; }
        public string blockID { get; set; }
        public string status { get; set; }
        public Accel accel { get; set; }
        public Gyro gyro { get; set; }

    }
}

