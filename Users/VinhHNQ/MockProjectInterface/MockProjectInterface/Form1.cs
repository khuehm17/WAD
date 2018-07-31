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
            txbThongTin.ScrollBars = ScrollBars.Vertical;
           
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
                    txbSend.Text = ("COM Port connected!!!\n");
                }
                else
                {
                    txbSend.Text = ("COM Port not found!!!\n");
                }
            }
        }


        private void button2_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                btnConnect.Enabled = true;
                txbSend.Text=("COM Port disconnected!!!\n");
            }
            else
            {
                txbSend.Text=("COM Port is not connected!!!\n");
                
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

                txbThongTin.Text = txbThongTin.Text+"\n"+serialPort1.ReadExisting();
                String[] strArray=serialPort1.ReadExisting().Split(' ');
                if (serialPort1.ReadExisting() )
                {
                    Button btn = new Button() { Width = 50, Height = 50 };
                    btn.Text = serialPort1.ReadExisting();
                    pnWheelChair.Controls.Add(btn);
                }
                txbThongTin.SelectionStart = txbThongTin.Text.Length;
                txbThongTin.ScrollToCaret();
            }
           
        }
    }
}
