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
        public Form1()
        {
            InitializeComponent();
            this.MinimumSize = new Size(735, 485);
            this.MaximumSize = new Size(735, 485);
            this.MaximizeBox = false;
           
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
                    txbThongTin.Text = ("COM Port connected!!!\n");
                }
                else
                {
                    txbThongTin.Text = ("COM Port not found!!!\n");
                }
            }
        }


        private void button2_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                btnConnect.Enabled = true;
                txbThongTin.Text=("COM Port disconnected!!!");
            }
            else
            {
                txbThongTin.Text=("COM Port is not connected!!!");
                
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

        private string AtoX(string asc)
        {
            int nLines;
            int nChars;
            int offset;
            string hex = " ";

            // tính toán số lượng dòng Hexa. 
            if ((asc.Length % 16) > 0)
                nLines = asc.Length / 16 + 1;
            else
                nLines = asc.Length / 16;

            // Chuyển đổi sang những dòng Hexa.
            for (int i = 0; i < nLines; i++)
            {
                offset = i * 16;
                if ((asc.Length - offset) > 16)
                    nChars = 16;
                else
                    nChars = asc.Length - offset;
                hex += this.HexLine(i, asc.Substring(offset, nChars)) + "\r\n";
            }
            return hex;
        }

        private string HexLine(int lNum, string asc)
        {
            string hex = "";

            // Copy dòng vào bộ đệm.
            char[] c = new char[16];
            asc.CopyTo(0, c, 0, asc.Length);

            // Tạo  offset.
            hex += String.Format("{0:X8} - {0:X8}", lNum * 16, (lNum + 1) * 16 - 1);
            hex += " ";

            // Chuyển các ký tự sang định dạng chuẩn hexa.
            for (int i = 0; i < asc.Length; i++)
            {
                if ((i != 0) && ((i % 4) == 0))
                    hex += " ";
                hex += String.Format("{0:X2}", (int)c[i]);
            }

            // Đệm thêm.
            int nSpaces = 62 - hex.Length;
            for (int i = 0; i < nSpaces; i++)
                hex += " ";

            //Chuyển ASCII tới cuối dòng.                                                                                                  
            for (int i = 0; i < asc.Length; i++)
            {
                if (((int)c[i] < 32) || ((int)c[i] > 126))
                    hex += ".";
                else
                    hex += c[i].ToString();
            }

            // Trả lại dòng hexa .
            return hex;
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {;
            MessageBox.Show("© 2018 WAD-Team All Rights Reserved");
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            txbThongTin.Text = txbThongTin + " " + txbSend.Text;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                txbThongTin.Text = txbThongTin.Text+"\n"+serialPort1.ReadExisting();
            }
        }
    }
}
