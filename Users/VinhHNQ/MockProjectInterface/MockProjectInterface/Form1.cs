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
using System.IO;
using OfficeOpenXml;
using System.Media;

namespace MockProjectInterface
{
    public partial class Form1 : Form
    {

        private List<CWheelChair> list = new List<CWheelChair>();

        public Form1()
        {
            InitializeComponent();
            this.MinimumSize = new Size(781, 485);
            this.MaximumSize = new Size(781, 485);
            this.MaximizeBox = false;
            txbThongTin.ScrollBars = ScrollBars.Vertical;
            LoadData();
            for (int i = 0; i < list.Count; i++)
            {
                int tmp = i * 34;
                CreateCustomTable(list[i], tmp);
            }

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
                    txbThongTin.Text = txbThongTin.Text + "\r\n COM Port connected!!!\r\n";
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
                txbThongTin.Text = txbThongTin.Text + "\r\n COM Port disconnected!!!\r\n";
            }
            else
            {
                txbThongTin.Text = txbThongTin.Text + "\r\n COM Port is not connected!!!\r\n";

            }
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


        private DialogResult dialogResult;

        private SoundPlayer soundPlayer = new SoundPlayer(@"..\..\RedAlert.wav");

        //Handle accident event
        private void accidentEvent(string accCode)
        {
            
            TextBox textBox = (TextBox)this.Controls.Find("txb" + accCode + "Status", true).FirstOrDefault();
            if (textBox != null)
            {
                this.Invoke(new MethodInvoker(delegate ()
                {
                    textBox.BackColor = Color.Red;
                   
                    soundPlayer.Play();
                 

                }));
                foreach (CWheelChair item in list)
                {
                    if (item.id == accCode) { item.status = "0"; }
                }
            }
        }

        private string getUserNamebyID(string id)
        {
            foreach (CWheelChair item in list)
            {
                if (item.id == id) { return item .userName ; }
            }
            return null;
        }

        private void txbThongTin_TextChanged(object sender, EventArgs e)
        {
            txbThongTin.SelectionStart = txbThongTin.Text.Length;
            txbThongTin.ScrollToCaret();
        }
        
        //Serial Port Data received event
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string tmp = serialPort1.ReadExisting(); 
            if (tmp != "")
            {
                if (tmp.Length < 7)
                {
                    String[] tmpArr = tmp.Split('_');
                    ProcessReceiveData(tmpArr[1],tmpArr[0]);
                }
                else {
                    string tmp2 = tmp.Remove(6);
                    String[] tmpArr = tmp2.Split('_');
                    ProcessReceiveData(tmpArr[1], tmpArr[0]);
                }
            }
        }

        //Determine the event
        private void ProcessReceiveData(string code, string id)
        {
            if (code == "AC")
            {
                accidentEvent(id);
                this.Invoke(new MethodInvoker(delegate () { txbThongTin.Text += "\r\n Accident signal incoming"; }));
            }
            else{
                stillOkEvent(id);
            }
        }

        //Handle still ok event 
        private void stillOkEvent(string id)
        {
            TextBox textBox = (TextBox)this.Controls.Find("txb" + id + "Status", true).FirstOrDefault();
            if (textBox != null)
            {
                this.Invoke(new MethodInvoker(delegate ()
                {
                    textBox.BackColor = Color.Green;
                    soundPlayer.Stop();
                
                }));
                foreach (CWheelChair item in list)
                {
                    if (item.id == id) { item.status = "1"; }
                }
            }
        }


        //Load data from excel
        private void LoadData()
        {  
                var package =new ExcelPackage(new FileInfo(@"..\..\ImportData.xlsx"));

                ExcelWorksheet worksheet = package.Workbook.Worksheets[1];

                for (int i = worksheet.Dimension.Start.Row + 1; i <= worksheet.Dimension.End.Row; i++)
                {


                    int j = 1;

                    string name = worksheet.Cells[i, j++].Value.ToString();
                    string userName = worksheet.Cells[i, j++].Value.ToString();
                    string roomID = worksheet.Cells[i, j++].Value.ToString();
                    string blockID = worksheet.Cells[i, j++].Value.ToString();
                    string status = worksheet.Cells[i, j++].Value.ToString();
                  

                    CWheelChair cWheel = new CWheelChair()
                    {
                        id = name,
                        userName = userName,
                        roomID = roomID,
                        blockID = blockID,
                        status = status
                    };
                       list.Add(cWheel);
                }
        }


        //Create a table of wheelchair data
        private void CreateCustomTable(CWheelChair wheelChair,int i)
        {
            Panel panel = new Panel();
            panel.Name = "pn"+wheelChair.id;
            panel.Location= new Point(3,24+i);
            panel.Size = new Size(537, 34);
            pnWheelChair.Controls.Add(panel);

            //Textbox Name
            TextBox textBox = new TextBox();
            textBox.Name = "txb"+wheelChair.id+"Name";
            textBox.Text = wheelChair.id;
            textBox.Location = new Point(0, 6);
            textBox.ReadOnly = true;
            textBox.Size = new Size(124, 20);
            textBox.Cursor = Cursors.Arrow;
            textBox.GotFocus += textBox_GotFocus;
            panel.Controls.Add(textBox);

            //Textbox User
            TextBox textbUser = new TextBox();
            textbUser.Name = "txb" + wheelChair.id+ "User";
            textbUser.Text = wheelChair.userName;
            textbUser.ReadOnly = true;
            textbUser.Location = new Point(130, 6);
            textbUser.Size = new Size(100, 20);
            textbUser.Cursor = Cursors.Arrow;
            textbUser.GotFocus += textBox_GotFocus;
            panel.Controls.Add(textbUser);

            //Textbox Room
            TextBox txbRoom = new TextBox();
            txbRoom.Name = "txb" + wheelChair.id+"Room" ;
            txbRoom.Text = wheelChair.roomID;
            txbRoom.ReadOnly = true;
            txbRoom.Location = new Point(236, 6);
            txbRoom.Size = new Size(100, 20);
            txbRoom.Cursor = Cursors.Arrow;
            txbRoom.GotFocus += textBox_GotFocus;
            panel.Controls.Add(txbRoom);

            //Textbox Block
            TextBox txbBlock = new TextBox();
            txbBlock.Name = "txb" + wheelChair.id+ "Block";
            txbBlock.Text = wheelChair.blockID;
            txbBlock.ReadOnly = true;
            txbBlock.Location = new Point(342, 6);
            txbBlock.Size = new Size(100, 20);
            txbBlock.Cursor = Cursors.Arrow;
            txbBlock.GotFocus += textBox_GotFocus;
            panel.Controls.Add(txbBlock);


            //Textbox Status
            TextBox txbStatus = new TextBox();
            txbStatus.Name = "txb" + wheelChair.id + "Status";
            txbStatus.ReadOnly = true;
            txbStatus.Location = new Point(448, 6);
            txbStatus.BackColor = Color.Green;
            txbStatus.Size = new Size(86, 20);
            txbStatus.Cursor = Cursors.Arrow;
            txbStatus.GotFocus += textBox_GotFocus;
            panel.Controls.Add(txbStatus);
        }

        private void textBox_GotFocus(object sender, EventArgs e)
        {
            ((TextBox)sender).Parent.Focus();
            
        }
    }
}
