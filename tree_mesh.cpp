#include "tree_mesh.h"

#include <cassert>
#include <list>
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

#include "cdagtree.h"
#include "math_utils.h"
#include "obj_exporter.h"

using namespace Eigen;

namespace SilhouetteTree{

	/*CircleStrip::CircleStrip(int aStartId,int aEndId,const RowVector3f& aDir):m_start_id(aStartId),m_end_id(aEndId),m_direction(aDir)
	{

	}
	CircleStrip::~CircleStrip()
	{

	}

	CircleStrip::CircleStrip(const CircleStrip& aNother):m_start_id(aNother.m_start_id),m_end_id(aNother.m_end_id),m_direction(aNother.m_direction)
	{
		
	}

	CircleStrip& CircleStrip::operator=(const CircleStrip& aNother)
	{
		if( this!=&aNother ){
			m_start_id=aNother.m_start_id;
			m_end_id=aNother.m_end_id;
			m_direction=aNother.m_direction;
		}

		return (*this);
	}

	int CircleStrip::findOppositPtId(int aGivenPtId)
	{
		int a_half_offset=(m_end_id-m_start_id)/2;
		if(aGivenPtId>=a_half_offset){
			return aGivenPtId-a_half_offset;
		}
		else
			return aGivenPtId+a_half_offset;
	}

	InternodeSurf::InternodeSurf(bool isJunction,UINT aInterNodeId):m_b_junction(isJunction),m_internode_id(aInterNodeId)
	{

	}

	InternodeSurf::~InternodeSurf()
	{

	}

	InternodeSurf::InternodeSurf(const InternodeSurf& aNother)
	{
		m_strip_list=aNother.m_strip_list;
		m_b_junction=aNother.m_b_junction;
		m_internode_id=aNother.m_internode_id;
	}

	InternodeSurf& InternodeSurf::operator=(const InternodeSurf& aNother)
	{
		if( this!=&aNother ){
			m_strip_list=aNother.m_strip_list;
			m_b_junction=aNother.m_b_junction;
			m_internode_id=aNother.m_internode_id;
		}

		return (*this);
	}

	void InternodeSurf::findTwoClosestPt(const vector<Vectorn<float> >& aVertlist,const CircleStrip& aC1,const CircleStrip& aC2,int& aPtId1,int aPtId2)
	{
		float a_minimum_dist=MAXIMUM;
		for(int i=aC1.getStartId();i<aC1.getStopId();++i){
			Vectorn<float> a_vert1=aVertlist[i];
			for(int j=aC2.getStartId();j<aC2.getStopId();++j){
				Vectorn<float> a_vert2=aVertlist[j];
				Vectorn<float> a_dist_vect=a_vert2-a_vert1;
				float a_squared_dist=a_dist_vect.squared_length();
				if( (a_squared_dist-a_minimum_dist)<EPSILON ){
					aPtId1=i;
					aPtId2=j;
					a_minimum_dist=a_squared_dist;
				}
			}
		}

	}

	int InternodeSurf::findTwoClosestPt(const vector<Vectorn<float> >& aVertList,const RowVector3f& aFixedPt,const CircleStrip& aTargetStrip)
	{
		float a_minimum_dist=MAXIMUM;
		int a_close_pt_id;
		for(int i=aTargetStrip.getStartId();i<aTargetStrip.getStopId();++i){
			Vectorn<float> a_vert=aVertList[i];
			RowVector3f a_pt(a_vert[0],a_vert[1],a_vert[2]);
			float a_dist=(a_pt-aFixedPt).norm();
			if( (a_dist-a_minimum_dist)<EPSILON ){
				a_minimum_dist=a_dist;
				a_close_pt_id=i;
			}
		}

		return a_close_pt_id;
	}

	void InternodeSurf::constructTriStrips(vector<Vectorn<int> >& aTriList,int aStartStripId,int aStopStripId)
	{
		for(int i=aStartStripId;i<aStopStripId;++i){
			int a_lower_start_id=m_strip_list[i].getStartId();
			int a_higher_start_id=m_strip_list[i+1].getStartId();
			//aTriList.push_back(Vectorn<int>(a_lower_start_id,a_higher_start_id+1,a_higher_start_id));

			//construct the triangle strips between two circle strips
			int j=0;
			for(;j<(m_strip_list[i].getStopId()-a_lower_start_id)-1;++j){
				aTriList.push_back(Vectorn<int>(a_lower_start_id+j,a_lower_start_id+j+1,a_higher_start_id+j+1));
				aTriList.push_back(Vectorn<int>(a_lower_start_id+j,a_higher_start_id+j+1,a_higher_start_id+j));
			}
			//close the triangle strip
			aTriList.push_back( Vectorn<int>(a_lower_start_id+j,a_lower_start_id,a_higher_start_id) );
			aTriList.push_back( Vectorn<int>(a_lower_start_id+j,a_higher_start_id,m_strip_list[i+1].getStopId()-1) );
		}
	}

	void InternodeSurf::constructMesh(vector<Vectorn<int> >& aTriList,const vector<Vectorn<float> >& aVertList,bool bColorBranch)
	{
		UINT a_color_id(m_internode_id);
		if(bColorBranch==false){
			a_color_id=1;
		}
		//case a normal inter-node: just connect the points among circle strip pairs
		if( m_b_junction==false ){
			//constructTriStrips(aTriList,0,m_strip_list.size()-1);
			for(int i=0;i<m_strip_list.size()-1;++i){
				int a_lower_start_id=m_strip_list[i].getStartId();
				int a_higher_start_id=m_strip_list[i+1].getStartId();
				//aTriList.push_back(Vectorn<int>(a_lower_start_id,a_higher_start_id+1,a_higher_start_id));

				//construct the triangle strips between two circle strips
				int j=0;
				for(;j<(m_strip_list[i].getStopId()-a_lower_start_id)-1;++j){
					aTriList.push_back(Vectorn<int>(a_lower_start_id+j,a_lower_start_id+j+1,a_higher_start_id+j+1,a_color_id));
					aTriList.push_back(Vectorn<int>(a_lower_start_id+j,a_higher_start_id+j+1,a_higher_start_id+j,a_color_id));
				}
				//close the triangle strip
				aTriList.push_back( Vectorn<int>(a_lower_start_id+j,a_lower_start_id,a_higher_start_id,a_color_id) );
				aTriList.push_back( Vectorn<int>(a_lower_start_id+j,a_higher_start_id,m_strip_list[i+1].getStopId()-1,a_color_id) );
			}
		}
		//case a junction: the technique presented in [Bloomenthal 1985] may not work!!!
	}*/


	TreeMesh::TreeMesh():m_maximum_axis_order(0),m_radius_scale_ratio(1.f)
	{

	}
	TreeMesh::~TreeMesh()
	{

	}
	TreeMesh::TreeMesh(const TreeMesh& aNother)
	{
		m_surf_list=aNother.m_surf_list;
		m_tri_list=aNother.m_tri_list;
		m_vert_list=aNother.m_vert_list;
		m_maximum_axis_order=aNother.m_maximum_axis_order;
		m_radius_scale_ratio=aNother.m_radius_scale_ratio;
	}

	TreeMesh& TreeMesh::operator=(const TreeMesh& aNother)
	{
		if( this!=&aNother ){
			m_surf_list=aNother.m_surf_list;
			m_tri_list=aNother.m_tri_list;
			m_vert_list=aNother.m_vert_list;
			m_maximum_axis_order=aNother.m_maximum_axis_order;
			m_radius_scale_ratio=aNother.m_radius_scale_ratio;
		}

		return (*this);
	}

	bool TreeMesh::saveToObjFile(const char* aFileName)
	{
		ObjExporter a_exporter;
		return a_exporter.writeObjFile(aFileName,m_tri_list,m_vert_list);
	}

	void TreeMesh::constructMeshRec(DAGNode* aNodePtr,const Matrix3f& aFirstFrame,const vector<CubicBsplineCurve>& aCurveList,int aAxisRes,int aCircleRes)
	{
		//Matrix3f a_end_frame;
		if( aNodePtr->m_child_list.empty() )
			return;

		//a normal inter-node
		if(aNodePtr->m_child_list.size()==1){
			DAGNode* a_child_node=(aNodePtr->m_child_list).front();
			InternodeSurf a_internode_surf(false,a_child_node->m_subtree_id+1);
			int a_curve_id=(aNodePtr->m_axis_id_list)[0];
			int a_span_id=aCurveList[a_curve_id].getSpanIdByNode(aNodePtr);
			assert(a_span_id!=-1);
			//a_end_frame=
			computeSurfaceVerticesAtInternode(a_internode_surf,aFirstFrame,aCurveList[a_curve_id],a_span_id,aAxisRes,0.f,1.f,aCircleRes);
			m_surf_list.push_back(a_internode_surf);
		}
		//a junction containing various inter-nodes
		else{
			//compute the junction part at current node
			computeSurfaceVerticesAtJunction(aNodePtr,aFirstFrame,aCurveList,aAxisRes,aCircleRes);

			/*InternodeSurf a_junction_surf(true);
			
			//compute the junction part at current node
			computeSurfaceVerticesAtJunction(a_junction_surf,aNodePtr,aFirstFrame,aCurveList,aAxisRes,0.f,0.5f,aCircleRes);
			m_surf_list.push_back(a_junction_surf);

			//compute the inter-node parts start at current node connecting the junction
			for(int i=0;i<aNodePtr->m_axis_id_list.size();++i){
				int a_curve_id=(aNodePtr->m_axis_id_list)[i];
				InternodeSurf a_internode_surf(false);
				int a_span_id=aCurveList[a_curve_id].getSpanIdByNode(aNodePtr);
				assert(a_span_id!=-1);
				computeSurfaceVerticesAtInternode(a_internode_surf,aFirstFrame,aCurveList[a_curve_id],a_span_id,aAxisRes,0.5f,1.f,aCircleRes);
				m_surf_list.push_back(a_internode_surf);
			}*/
		}

		list<DAGNode*>::iterator a_start_iter=aNodePtr->m_child_list.begin();
		for(;a_start_iter!=aNodePtr->m_child_list.end();++a_start_iter){
			//if( aCurveList[a_curve_id].isNodeInCurve((*a_start_iter))==true )
			constructMeshRec((*a_start_iter),aFirstFrame,aCurveList,aAxisRes,aCircleRes);
			//else
				//constructMeshRec((*a_start_iter),a_end_fram)
		}
	}

	void TreeMesh::constructMeshByInterNode(DAGNode* aRootNodePtr,const vector<CubicBsplineCurve>& aCurveList,int aAxisRes,int aCircleRes)
	{
		int a_curve_id=(aRootNodePtr->m_axis_id_list)[0];
		Matrix3f a_initial_frame=computeInitalFrenetFrame(aCurveList[a_curve_id]);

		constructMeshRec(aRootNodePtr,a_initial_frame,aCurveList,aAxisRes,aCircleRes);

		m_maximum_axis_order=20;
		//finally construct the mesh for each inter-node
		vector<InternodeSurf>::iterator a_start_iter=m_surf_list.begin();
		for(;a_start_iter!=m_surf_list.end();++a_start_iter){
			a_start_iter->constructMesh(m_tri_list,m_vert_list);
		}
		
	}


	void TreeMesh::constructMeshByCurve(DAGNode* aRootNodePtr,const std::vector<CubicBsplineCurve>& aCurveList,int aAxisRes,int aCircleRes,bool bColorBranch)
	{
		int a_curve_id=(aRootNodePtr->m_axis_id_list)[0];
		Matrix3f a_initial_frame=computeInitalFrenetFrame(aCurveList[a_curve_id]);

		vector<CubicBsplineCurve>::const_iterator a_curve_iter=aCurveList.begin();
		for(;a_curve_iter!=aCurveList.end();++a_curve_iter){
			
			if( m_maximum_axis_order<a_curve_iter->getCurveId() )
				m_maximum_axis_order=a_curve_iter->getCurveId();

			computeCurveSurfaceVertices((*a_curve_iter),a_initial_frame,aAxisRes,aCircleRes);
		}

		//finally construct the mesh for each inter-node
		vector<InternodeSurf>::iterator a_start_iter=m_surf_list.begin();
		for(;a_start_iter!=m_surf_list.end();++a_start_iter){
			a_start_iter->constructMesh(m_tri_list,m_vert_list,bColorBranch);
		}
	}

	void TreeMesh::constructMeshByLineSegments(DAGNode* aRootNodePtr,const vector<CubicBsplineCurve>& aCurveList,int aAxisRes,int aCircleRes)
	{
		//compute a start local frame at the root node
		Matrix3f a_start_frame;
		int a_curve_id=(aRootNodePtr->m_axis_id_list)[0];
		DAGNode* a_child_node=(aCurveList[a_curve_id].getKnotList())[1];
		Vectorn<FLOAT> a_dir=a_child_node->m_junction_pos-aRootNodePtr->m_junction_pos;
		RowVector3f a_dir_vect(a_dir[0],a_dir[1],a_dir[2]);
		a_dir_vect.normalize();
		constructLocalFrame(a_start_frame,a_dir_vect);

		//iterate each curve to compute surface 
		vector<CubicBsplineCurve>::const_iterator a_curve_iter=aCurveList.begin();
		for(;a_curve_iter!=aCurveList.end();++a_curve_iter){
			const vector<DAGNode*>& a_knot_list=a_curve_iter->getKnotList();
			InternodeSurf a_curve_surf(false,a_curve_iter->getCurveId());
			
			if( m_maximum_axis_order<a_curve_iter->getCurveId() )
				m_maximum_axis_order=a_curve_iter->getCurveId();

			//for each line segments between two successive nodes along the curve
			for(int i=0;i<a_knot_list.size()-1;++i){
				DAGNode* a_current_node=a_knot_list[i];
				DAGNode* a_next_node=a_knot_list[i+1];
				Vectorn<FLOAT> a_line_dir=a_next_node->m_junction_pos-a_current_node->m_junction_pos;
				RowVector3f a_lead_dir(a_line_dir[0],a_line_dir[1],a_line_dir[2]);
				a_lead_dir.normalize();
				Matrix3f a_segment_frame;
				computeFrameAtNode(a_segment_frame,a_start_frame,a_lead_dir);

				//create the surface vertices between node i and node i+1
				computeCurveSurfaceVerticesAtLineSegment(a_curve_surf,a_segment_frame,a_current_node,a_next_node,aAxisRes,aCircleRes);	
			}
			m_surf_list.push_back(a_curve_surf);
		}

		//finally construct the mesh for each inter-node
		vector<InternodeSurf>::iterator a_start_iter=m_surf_list.begin();
		for(;a_start_iter!=m_surf_list.end();++a_start_iter){
			a_start_iter->constructMesh(m_tri_list,m_vert_list);
		}

	}

	void TreeMesh::constructMeshByInterNodeLineSegments(DAGNode* aRootNodePtr,const std::vector<CubicBsplineCurve>& aCurveList,int aAxisRes,int aCircleRes)
	{
		//compute a start local frame at the root node
		Matrix3f a_start_frame;
		int a_curve_id=(aRootNodePtr->m_axis_id_list)[0];
		DAGNode* a_child_node=(aCurveList[a_curve_id].getKnotList())[1];
		Vectorn<FLOAT> a_dir=a_child_node->m_junction_pos-aRootNodePtr->m_junction_pos;
		RowVector3f a_dir_vect(a_dir[0],a_dir[1],a_dir[2]);
		a_dir_vect.normalize();
		constructLocalFrame(a_start_frame,a_dir_vect);
		m_maximum_axis_order=20;

		//iterate each curve to compute surface 
		vector<CubicBsplineCurve>::const_iterator a_curve_iter=aCurveList.begin();
		for(;a_curve_iter!=aCurveList.end();++a_curve_iter){
			const vector<DAGNode*>& a_knot_list=a_curve_iter->getKnotList();

			//for each line segments between two successive nodes along the curve
			for(int i=0;i<a_knot_list.size()-1;++i){
				DAGNode* a_current_node=a_knot_list[i];
				DAGNode* a_next_node=a_knot_list[i+1];
				Vectorn<FLOAT> a_line_dir=a_next_node->m_junction_pos-a_current_node->m_junction_pos;
				RowVector3f a_lead_dir(a_line_dir[0],a_line_dir[1],a_line_dir[2]);
				a_lead_dir.normalize();
				Matrix3f a_segment_frame;
				computeFrameAtNode(a_segment_frame,a_start_frame,a_lead_dir);

				//create the surface vertices between node i and node i+1
				InternodeSurf a_curve_surf(false,a_next_node->m_subtree_id+1);
				computeLineSurfaceVerticesAtInternode(a_curve_surf,a_segment_frame,a_current_node,a_next_node,aAxisRes,aCircleRes);	
				m_surf_list.push_back(a_curve_surf);

			}
		}

		//finally construct the mesh for each inter-node
		vector<InternodeSurf>::iterator a_start_iter=m_surf_list.begin();
		for(;a_start_iter!=m_surf_list.end();++a_start_iter){
			a_start_iter->constructMesh(m_tri_list,m_vert_list);
		}
	}


	void TreeMesh::computeCircleStripAt(InternodeSurf& aAxisSurface,
										const Matrix3f& aFrame,
										const RowVector3f& aDir,
										const RowVector3f& aPos,
										INT aCircleRes,
										FLOAT aRadius)
	{
		Matrix3f a_transform_mat;
		a_transform_mat.row(0)=aPos;
		a_transform_mat.row(1)=aFrame.row(1);
		a_transform_mat.row(2)=aFrame.row(2);
		INT a_vert_size=m_vert_list.size();
		aAxisSurface.addCircleStrip( CircleStrip(a_vert_size,a_vert_size+aCircleRes,aDir) );
		FLOAT a_rad_step=2*PI/aCircleRes;
		for(INT i=0;i<aCircleRes;++i){
			RowVector3f a_local_sample(1.f,aRadius*cos(i*a_rad_step),aRadius*sin(i*a_rad_step));
			RowVector3f a_global_sample=a_local_sample*a_transform_mat;
			//debug the transformation whether preserve the radius
			//RowVector3f a_diff_vect=a_global_sample-aPos;
			//FLOAT a_len=a_diff_vect.norm();
			m_vert_list.push_back(Vectorn<FLOAT>(a_global_sample(0),a_global_sample(1),a_global_sample(2)));
		}
		

	}


	void TreeMesh::computeCurveSurfaceVerticesAtLineSegment(InternodeSurf& aAxisSurface,
															const Matrix3f& aFrame,
															DAGNode* aStartNodePtr,
															DAGNode* aStopNodePtr,
															int aAxisRes,
															int aCircleRes)
	{
		/*Vectorn<FLOAT> a_line_segment=aStopNodePtr->m_junction_pos-aStartNodePtr->m_junction_pos;
		RowVector3f a_dir_vect(a_line_segment[0],a_lin)
		FLOAT a_step_dist=a_line_segment.length()/FLOAT(aAxisRes);*/

		RowVector3f a_start_pos((aStartNodePtr->m_junction_pos)[0],(aStartNodePtr->m_junction_pos)[1],(aStartNodePtr->m_junction_pos)[2]);
		RowVector3f a_stop_pos((aStopNodePtr->m_junction_pos)[0],(aStopNodePtr->m_junction_pos)[1],(aStopNodePtr->m_junction_pos)[2]);
		RowVector3f a_dir=a_stop_pos-a_start_pos;
		FLOAT a_weight=FLOAT(1)/FLOAT(aAxisRes);
		for(int t=0;t<aAxisRes;++t){
			FLOAT a_radius=(1.f-t*a_weight)*(aStartNodePtr->m_parent_pow_radius)+(t*a_weight)*(aStopNodePtr->m_parent_pow_radius);
			RowVector3f a_sample_pos=(1.f-t*a_weight)*a_start_pos+t*a_weight*a_stop_pos;
			//compute the circle strip at current position
			computeCircleStripAt(aAxisSurface,aFrame,a_dir,a_sample_pos,aCircleRes,a_radius*m_radius_scale_ratio);
		}
	}

	void TreeMesh::computeLineSurfaceVerticesAtInternode(InternodeSurf& aAxisSurface,
														const Eigen::Matrix3f& aFrame,
														DAGNode* aStartNodePtr,
														DAGNode* aStopNodePtr,
														int aAxisRes,
														int aCircleRes)
	{
		RowVector3f a_start_pos((aStartNodePtr->m_junction_pos)[0],(aStartNodePtr->m_junction_pos)[1],(aStartNodePtr->m_junction_pos)[2]);
		RowVector3f a_stop_pos((aStopNodePtr->m_junction_pos)[0],(aStopNodePtr->m_junction_pos)[1],(aStopNodePtr->m_junction_pos)[2]);
		RowVector3f a_dir=a_stop_pos-a_start_pos;
		FLOAT a_ratio=1.f/2.3f;
		FLOAT a_start_radius=pow(aStartNodePtr->m_parent_pow_radius,a_ratio);
		FLOAT a_stop_radius=pow(aStopNodePtr->m_parent_pow_radius,a_ratio);

		FLOAT a_weight=FLOAT(1)/FLOAT(aAxisRes);
		for(int t=0;t<=aAxisRes;++t){
			FLOAT a_radius=(1.f-t*a_weight)*(a_start_radius)+(t*a_weight)*(a_stop_radius);
			RowVector3f a_sample_pos=(1.f-t*a_weight)*a_start_pos+t*a_weight*a_stop_pos;
			//compute the circle strip at current position
			computeCircleStripAt(aAxisSurface,aFrame,a_dir,a_sample_pos,aCircleRes,a_radius*m_radius_scale_ratio);
		}
	}

	void TreeMesh::constructLocalFrame(Matrix3f& aLocalFrame,const RowVector3f& aFirstAxis)
	{
		//find a second axis by first set the element with minimum magnitude zero
		//then swap the remaining two elements and negative one of them
		RowVector3f a_first_vect=aFirstAxis;
		int a_min_elem_index;
		a_first_vect.cwiseAbs().minCoeff(&a_min_elem_index);
		RowVector3f a_second_vect=a_first_vect;
		a_second_vect(a_min_elem_index)=0;
		
		if(a_min_elem_index==0){
			swap(a_second_vect(1),a_second_vect(2));
			a_second_vect(1)=-a_second_vect(1);
		}
		else if(a_min_elem_index==1){
			swap(a_second_vect(0),a_second_vect(2));
			a_second_vect(0);
		}
		else if(a_min_elem_index==2){
			swap(a_second_vect(0),a_second_vect(1));
			a_second_vect(0)=-a_second_vect(0);
		}
		a_second_vect.normalize();
		
		RowVector3f a_third_vect=a_first_vect.cross(a_second_vect);
		a_third_vect.normalize();

		aLocalFrame.row(0)=a_first_vect;
		aLocalFrame.row(1)=a_second_vect;
		aLocalFrame.row(2)=a_third_vect;
	}

	void TreeMesh::computeCurveSurfaceVertices(const CubicBsplineCurve& aCurve,
												const Matrix3f& aFirstFrame,
												int aSpanAxisRes,
												int aSpanCircleRes)
	{
		float a_dist_step=1.f/aSpanAxisRes;
		InternodeSurf a_curve_surf(false,aCurve.getCurveId());
		for(int s=0;s<aCurve.getSpanNums();++s){
			const CubicBsplineSpan& a_span=aCurve.getSpanAt(s);
			for(int t=0;t<aSpanAxisRes;++t){
				computeCircleStrip(a_curve_surf,aFirstFrame,a_span,t*a_dist_step,aSpanCircleRes);
			}
		}
		m_surf_list.push_back(a_curve_surf);		
	}

	Matrix3f TreeMesh::computeInitalFrenetFrame(const CubicBsplineCurve& aCurve)
	{
		//construct the frenet frame
		Matrix3f a_frame;

		//compute the first and second derivative vector
		RowVector3f a_velocity=aCurve.getSpanAt(0).getTangentAt(0.f);
		a_velocity.normalize();
		
		RowVector3f a_acceleration=aCurve.getSpanAt(0).getAccelerateAt(0.f);
		if( a_acceleration.norm()<EPSILON ){
			constructLocalFrame(a_frame,a_velocity);
			cout<<"The initial frame is: "<<a_frame<<endl;
			return a_frame;
		}

		a_acceleration.normalize();

		//compute the bi-normal
		RowVector3f a_bi_normal=a_velocity.cross(a_acceleration);
		a_bi_normal.normalize();

		//compute the normal
		RowVector3f a_normal=a_bi_normal.cross(a_velocity);
		a_normal.normalize();
		
		a_frame.row(0)=a_velocity;
		a_frame.row(1)=a_normal;
		a_frame.row(2)=a_bi_normal;

		cout<<"The initial frame is: "<<a_frame<<endl;
		return a_frame;
	}

	Matrix3f TreeMesh::computeFrenetFrame(const Matrix3f& aFirstFrame,const CubicBsplineSpan& aCurveSpan,float t)
	{
		//The matrix of the first frame is in a row-major fashion, thus the row0 is T,row 1 is N and row 2 is B

		assert(/*s>=0 && s<aCurve.getSpanNums() &&*/ t>=0.f && (t-1.f)<EPSILON);

		RowVector3f a_velocity=aCurveSpan.getTangentAt(t);//aCurve.getSpanAt(s).getTangentAt(t);
		a_velocity.normalize();
		//cout<<"The velocity is(T): "<<a_velocity<<endl;
		
		RowVector3f a_rotation_axis=aFirstFrame.row(0).cross(a_velocity);

		//in case the curve is nearly a straight line then just return the same frame
		if( a_rotation_axis.norm()<EPSILON )
			return aFirstFrame;

		a_rotation_axis.normalize();
		//cout<<"The rotation_axis is(A): "<<a_rotation_axis<<endl;
		/*Matrix3f a_old_basis_mat_t;
		a_old_basis_mat_t.row(0)=aFirstFrame.row(0);
		a_old_basis_mat_t.row(1)=a_rotation_axis;
		a_old_basis_mat_t.row(2)=a_old_basis_mat_t.row(0).cross(a_old_basis_mat_t.row(1));

		Matrix3f a_new_basis_mat;
		a_new_basis_mat.col(0)=a_velocity.transpose();
		a_new_basis_mat.col(1)=a_rotation_axis.transpose();
		a_new_basis_mat.col(2)=a_new_basis_mat.col(0).cross(a_new_basis_mat.col(1));

		Matrix3f a_rotation_mat=a_old_basis_mat_t*a_new_basis_mat;*/
	
		//compute the old basis (T0,A,A0)
		Matrix3f a_old_basis_mat; 
		a_old_basis_mat.col(0)=aFirstFrame.row(0).transpose();
		a_old_basis_mat.col(1)=a_rotation_axis.transpose();
		a_old_basis_mat.col(2)=a_old_basis_mat.col(0).cross(a_old_basis_mat.col(1));
		a_old_basis_mat.col(2).normalize();
		//cout<<"The old basis: "<<a_old_basis_mat<<endl;

		//compute the new basis(T1,A,A1)
		Matrix3f a_new_basis_mat_t;
		a_new_basis_mat_t.row(0)=a_velocity;
		a_new_basis_mat_t.row(1)=a_rotation_axis;
		a_new_basis_mat_t.row(2)=a_new_basis_mat_t.row(0).cross(a_new_basis_mat_t.row(1));
		a_new_basis_mat_t.row(2).normalize();
		//cout<<"The new basis(transposed): "<<a_new_basis_mat_t<<endl;

		Matrix3f a_rotation_mat=a_old_basis_mat*a_new_basis_mat_t;

		//compute the new frame (T',N',B')
		// N' is computed as follows:
		// (1) compute the local coordinates in the old basis (T0 A A0);
		// (2) this local coordinates is the same as in the new basis (T1 A A1) and use it to compute the global
		//		coordinates;
		a_new_basis_mat_t.row(0)=a_velocity;
		a_new_basis_mat_t.row(1)=(aFirstFrame.row(1)*a_rotation_mat).normalized();
		a_new_basis_mat_t.row(2)=(aFirstFrame.row(2)*a_rotation_mat).normalized();
		
		//cout<<"The final local frame: "<<a_new_basis_mat_t<<endl;
		return a_new_basis_mat_t;
	}

	void TreeMesh::computeFrameAtNode(Matrix3f& aFrame,const Matrix3f& aRefFrame,const RowVector3f& aLeadDir)
	{
		RowVector3f a_rotation_axis=aRefFrame.row(0).cross(aLeadDir);
		if( a_rotation_axis.norm()<EPSILON ){
			aFrame=aRefFrame;
		}
		else{
			a_rotation_axis.normalize();

			//create two local frames
			Matrix3f a_previous_frame;
			a_previous_frame.col(0)=aRefFrame.row(0).transpose();
			a_previous_frame.col(1)=a_rotation_axis.transpose();
			a_previous_frame.col(2)=a_previous_frame.col(0).cross(a_previous_frame.col(1)).normalized();

			Matrix3f a_current_frame;
			a_current_frame.row(0)=aLeadDir;
			a_current_frame.row(1)=a_rotation_axis;
			a_current_frame.row(2)=a_current_frame.row(0).cross(a_current_frame.row(1)).normalized();

			//compute the rotation matrix
			Matrix3f a_rotation_mat;
			a_rotation_mat=a_previous_frame*a_current_frame;

			aFrame.row(0)=aLeadDir;
			aFrame.row(1)=(aRefFrame.row(1)*a_rotation_mat).normalized();
			aFrame.row(2)=(aRefFrame.row(2)*a_rotation_mat).normalized();

		}
	}

	void TreeMesh::computeSurfaceVerticesAtInternode(InternodeSurf& aInterSurf,const Matrix3f& aFirstFrame,
													const CubicBsplineCurve& aCurve,
												    int aSpanId,int aAxisRes,float aAxisResStartRatio,
												    float aAxisResStopRatio,int aCircleRes)
	{
		float a_step_dist=1.f/aAxisRes;

		//for(int s=0;s<aCurve.getSpanNums();++s){
		const CubicBsplineSpan& a_span=aCurve.getSpanAt(aSpanId);
		int a_start=int(aAxisRes*aAxisResStartRatio);
		int a_stop=int(aAxisRes*aAxisResStopRatio);
		for(int i=a_start;i<=a_stop;++i){
			float t=i*a_step_dist;
			computeCircleStrip(aInterSurf,aFirstFrame,a_span,t,aCircleRes);
		
			/*RowVector3f a_sample_pt=a_span.samplePtAt(t);
			
			//compute the local Frenet Frame at current pos from the reference frame
			a_ref_frame=computeFrenetFrame(aFrame,a_span,t);

			//store current circle
			int a_vert_size=m_vert_list.size();
			//m_circle_strip_list.push_back(CircleStrip(a_vert_size,a_vert_size+aCircleRes,a_ref_frame.row(0)));
				
			aInterSurf.addCircleStrip(CircleStrip(a_vert_size,a_vert_size+aCircleRes,a_ref_frame.row(0)));

			//change the frame to (P N B) to transform a local pt to global
			a_ref_frame.row(0)=a_sample_pt;

			//interpolate the radius at current pos
			float a_radius=a_span.getRadiusAt(t);
				
			//sample the local circle
			for(int i=0;i<aCircleRes;++i){
				RowVector3f a_local_pt(1.f,a_radius*cos(i*a_step_rad),a_radius*sin(i*a_step_rad));
				RowVector3f a_global_pt=a_local_pt*a_ref_frame;
				m_vert_list.push_back(Vectorn<float>(a_global_pt(0),a_global_pt(1),a_global_pt(2)));
			}*/
		}
			
		//}
	}


	void TreeMesh::computeSurfaceVerticesAtJunction(InternodeSurf& aInterSurf,
													DAGNode* aNodePtr,
													const Matrix3f& aFirstFrame,
													const vector<CubicBsplineCurve>& aCurveList,
													int aAxisRes,
													float aAxisResStartRatio,
													float aAxisResStopRatio,
													int aCircleRes)
	{
		
		float a_step_dist=1.f/aAxisRes;
		
		int a_start=int(aAxisRes*aAxisResStartRatio);
		int a_stop=int(aAxisRes*aAxisResStopRatio);
		
		int a_curve_id=(aNodePtr->m_axis_id_list)[0];
		int a_span_id=aCurveList[a_curve_id].getSpanIdByNode(aNodePtr);
		assert(a_span_id!=-1);
		//in current case the front curve id is to the parent curve
		const CubicBsplineSpan& a_span=aCurveList[0].getSpanAt(a_span_id);
		computeCircleStrip(aInterSurf,aFirstFrame,a_span,a_start*a_step_dist,aCircleRes);
		
		for(int i=0;i<aNodePtr->m_axis_id_list.size();++i){
			a_curve_id=(aNodePtr->m_axis_id_list)[i];
			a_span_id=aCurveList[a_curve_id].getSpanIdByNode(aNodePtr);
			assert(a_span_id!=-1);
			const CubicBsplineSpan& a_span=aCurveList[a_curve_id].getSpanAt(a_span_id);

			computeCircleStrip(aInterSurf,aFirstFrame,a_span,a_step_dist*a_stop,aCircleRes);
		}
		/*const CubicBsplineSpan& a_span=aCurve.getSpanAt(aSpanId);
		
		//
		computeCircleStrip(aInterSurf,aFirstFrame,a_span,a_start*a_step_dist,aCircleRes);

		computeCircleStrip(aInterSurf,aFirstFrame,a_span,a_step_dist*a_stop,aCircleRes);*/
	}

	void TreeMesh::computeSurfaceVerticesAtJunction(DAGNode* aNodePtr,
												    const Matrix3f& aFirstFrame,
											        const vector<CubicBsplineCurve>& aCurveList,
											        int aAxisRes,
											        int aCircleRes)
	{
		float a_step_dist=1.f/aAxisRes;
		int a_curve_id=(aNodePtr->m_axis_id_list)[0];
		int a_span_id=aCurveList[a_curve_id].getSpanIdByNode(aNodePtr);
		assert(a_span_id!=-1);
		//in current case the front curve id is to the parent curve
		const CubicBsplineSpan& a_span=aCurveList[0].getSpanAt(a_span_id);
		
		CircleStrip a_root_strip=computeCircleStrip(aFirstFrame,a_span,0*a_step_dist,aCircleRes);
		
		for(int i=0;i<aNodePtr->m_axis_id_list.size();++i){
			a_curve_id=(aNodePtr->m_axis_id_list)[i];
			a_span_id=aCurveList[a_curve_id].getSpanIdByNode(aNodePtr);
			assert(a_span_id!=-1);
			const CubicBsplineSpan& a_span=aCurveList[a_curve_id].getSpanAt(a_span_id);

			//!Now just construct the circle strip along the curve
			//!!!For future improvements there may need some modification
			//computeCircleStrip(aInterSurf,aFirstFrame,a_span,a_step_dist*a_stop,aCircleRes);
			DAGNode* a_end_node=aCurveList[a_curve_id].getEndNodeAtSpan(a_span_id);
			InternodeSurf a_internode_surf(false,a_end_node->m_subtree_id+1);
			//a_internode_surf.addCircleStrip(a_root_strip);
			for(int j=0;j<=aAxisRes;++j){
				computeCircleStrip(a_internode_surf,aFirstFrame,a_span,a_step_dist*j,aCircleRes);
			}

			m_surf_list.push_back(a_internode_surf);
		}
	}


	void TreeMesh::computeCircleStrip(InternodeSurf& aInterSurf,
										const Matrix3f& aFirstFrame,
										const CubicBsplineSpan& aCurveSpan,
										float t,int aCircleRes)
	{
		//compute the position at parameter t on the curve span
		RowVector3f a_sample_pt=aCurveSpan.samplePtAt(t);
		//compute the local frame at the sample position
		Matrix3f a_ref_frame=computeFrenetFrame(aFirstFrame,aCurveSpan,t);
		//store the current circle-strip
		int a_vert_size=m_vert_list.size();
		aInterSurf.addCircleStrip(CircleStrip(a_vert_size,a_vert_size+aCircleRes,a_ref_frame.row(0)));
		//construct a transform matrix to convert a local point to a global one
		a_ref_frame.row(0)=a_sample_pt;
		//interpolate the radius at current position
		float a_radius=aCurveSpan.getRadiusAt(t)*m_radius_scale_ratio;
		//sample a local circle then convert the samples to global coordinates
		float a_unit_rad=2*PI/aCircleRes;
		for(int i=0;i<aCircleRes;++i){
			RowVector3f a_local_pt(1.f,a_radius*cos(a_unit_rad*i),a_radius*sin(a_unit_rad*i));
			RowVector3f a_global_pt=a_local_pt*a_ref_frame;
			m_vert_list.push_back(Vectorn<float>(a_global_pt(0),a_global_pt(1),a_global_pt(2)));
		}
	}

	CircleStrip TreeMesh::computeCircleStrip(const Matrix3f& aFirstFrame,const CubicBsplineSpan& aCurveSpan,float t,int aCircleRes)
	{
		//compute the position at parameter t on the curve span
		RowVector3f a_sample_pt=aCurveSpan.samplePtAt(t);
		//compute the local frame at the sample position
		Matrix3f a_ref_frame=computeFrenetFrame(aFirstFrame,aCurveSpan,t);
		//store the current circle-strip
		int a_vert_size=m_vert_list.size();
		CircleStrip a_strip(a_vert_size,a_vert_size+aCircleRes,a_ref_frame.row(0));
		//construct a transform matrix to convert a local point to a global one
		a_ref_frame.row(0)=a_sample_pt;
		//interpolate the radius at current position
		float a_radius=aCurveSpan.getRadiusAt(t)*m_radius_scale_ratio;
		//sample a local circle then convert the samples to global coordinates
		float a_unit_rad=2*PI/aCircleRes;
		for(int i=0;i<aCircleRes;++i){
			RowVector3f a_local_pt(1.f,a_radius*cos(a_unit_rad*i),a_radius*sin(a_unit_rad*i));
			RowVector3f a_global_pt=a_local_pt*a_ref_frame;
			m_vert_list.push_back(Vectorn<float>(a_global_pt(0),a_global_pt(1),a_global_pt(2)));
		}
		return a_strip;
	}
}//end namespace