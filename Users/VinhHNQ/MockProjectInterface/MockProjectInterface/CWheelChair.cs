using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MockProjectInterface
{
    public class CWheelChair
    {
        private static CWheelChair instance;

        public string id { get; set; }
        public string userName { get; set; }
        public string roomID { get; set; }
        public string blockID { get; set; }
        public Accel accel { get; set; }
        public Gyro gyro { get; set; }

        private CWheelChair() { }

        public static CWheelChair getInstance()
        {
            if (instance == null)
            {
                instance = new CWheelChair();
            }
            return instance;
        }
    }
}

