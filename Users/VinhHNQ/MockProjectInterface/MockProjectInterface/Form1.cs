using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;



namespace MockProjectInterface
{
    public partial class Form1 : Form
    {
        private CWheelChair wheelChair = CWheelChair.getInstance();
        public Form1()
        {
            InitializeComponent();
            this.MinimumSize = new Size(781, 485);
            this.MaximumSize = new Size(781, 485);
            this.MaximizeBox = false;
            txbThongTin.ScrollBars = ScrollBars.Vertical;
            txbStatus.BackColor = Color.Green;

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox1.DataSource = SerialPort.GetPortNames();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
            {
                if (comboBox1.Text != "")
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.Open();
                    btnConnect.Enabled = false;
                    txbThongTin.Text = txbThongTin.Text +"\r\n COM Port connected!!!\r\n";
                }
                else
                {
                    txbThongTin.Text = txbThongTin.Text + "\r\n COM Port not found!!!\r\n";
                }
            }
        }


        private void button2_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                btnConnect.Enabled = true;
                txbThongTin.Text = txbThongTin.Text  + "\r\n COM Port disconnected!!!\r\n";
            }
            else
            {
                txbThongTin.Text = txbThongTin.Text + "\r\n COM Port is not connected!!!\r\n";

            }
        }


        private void ReceiveLoraData(Object sender, SerialDataReceivedEventArgs e)
        {
            serialPort1.DataReceived += new SerialDataReceivedEventHandler(ReceiveLoraData);
            string readData = serialPort1.ReadExisting().ToString();
        }

        private void HandleReceivedData(String buffer)
        {
            String[] strArray = buffer.Split('_');

        }

        private void button3_Click(object sender, EventArgs e)
        {
            comboBox1.DataSource = SerialPort.GetPortNames();
        }


        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {;
            MessageBox.Show("© 2018 WAD-Team All Rights Reserved");
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            txbThongTin.Clear();

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {

                txbThongTin.Text = txbThongTin.Text + "\r\n" + serialPort1.ReadExisting();
              
            }
        }

        private void accidentEvent(string accCode)
        {
            String[] strArray = accCode.Split(' ');
            wheelChair.id = strArray[0];
            wheelChair.userName = strArray[1];
            wheelChair.roomID = strArray[2];
            wheelChair.blockID = strArray[3];
            //switch (strArray[4])
            //{
            //    case "Accel":
            //        wheelChair.accel.X = Int32.Parse(strArray[5]);
            //        wheelChair.accel.Y = Int32.Parse(strArray[6]);
            //        wheelChair.accel.Z = Int32.Parse(strArray[7]);
            //        break;
            //    case "Gyro":
            //        wheelChair.gyro.X = Int32.Parse(strArray[5]);
            //        wheelChair.gyro.Y = Int32.Parse(strArray[6]);
            //        wheelChair.gyro.Z = Int32.Parse(strArray[7]);
            //        break;
            //    default:
            //        break;
            //}

            //switch (strArray[8])
            //{
            //    case "Accel":
            //        wheelChair.accel.X = Int32.Parse(strArray[9]);
            //        wheelChair.accel.Y = Int32.Parse(strArray[10]);
            //        wheelChair.accel.Z = Int32.Parse(strArray[11]);
            //        break;
            //    case "Gyro":
            //        wheelChair.gyro.X = Int32.Parse(strArray[9]);
            //        wheelChair.gyro.Y = Int32.Parse(strArray[10]);
            //        wheelChair.gyro.Z = Int32.Parse(strArray[11]);
            //        break;
            //    default:
            //        break;
            //}

            SetTxbName(wheelChair.id.ToString());
            SetTxbUser(wheelChair.userName);
            SetTxbRoom(wheelChair.roomID);
            SetTxbBlock(wheelChair.blockID);
           
        }

        delegate void SetTextCallback(string text);


        private void SetTxbName(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txbName.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetTxbName);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txbName.Text = text;
            }
        }

        private void SetTxbUser(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txbUser.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetTxbUser);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txbUser.Text = text;
            }
        }

        private void SetTxbRoom(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txbRoom.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetTxbRoom);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txbRoom.Text = text;
            }
        }

        private void SetTxbBlock(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txbBlock.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetTxbBlock);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txbBlock.Text = text;
            }
        }

        private void txbThongTin_TextChanged(object sender, EventArgs e)
        {
            txbThongTin.SelectionStart = txbThongTin.Text.Length;
            txbThongTin.ScrollToCaret();
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string tmp = serialPort1.ReadExisting(); ;
            if (tmp != "")
            {
               
                accidentEvent(tmp);
            }
        }

        private void txbName_TextChanged(object sender, EventArgs e)
        {
            txbStatus.BackColor = Color.Red;
        }
    }
}
