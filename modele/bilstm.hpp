#ifndef BILSTM_HPP
#define BILSTM_HPP

        #include "dynet/nodes.h"
        #include "dynet/dynet.h"
        #include "dynet/training.h"
        #include "dynet/timing.h"
        #include "dynet/rnn.h"
        #include "dynet/gru.h"
        #include "dynet/lstm.h"
        #include "dynet/dict.h"
        #include "dynet/expr.h"
        #include "dynet/cfsm-builder.h"
        #include "dynet/hsm-builder.h"
        #include "dynet/globals.h"
        #include "dynet/io.h"
        #include "../code_data/data.hpp"

        /**
         * \file bilstm.hpp
        */


        /** 
         * \class BiLSTM
         * \brief Class representing a bidirectional LSTM
        */
        class BiLSTM
        {
                private:
                /*Attributes*/
                        dynet::Parameter p_W; /*!< weight*/ 
                        dynet::Parameter p_bias; /*!< bias*/
                        dynet::VanillaLSTMBuilder* forward_lstm; /*!< forward LSTM*/ 
                        dynet::VanillaLSTMBuilder* backward_lstm; /*!< backward LSTM*/ 
                        unsigned nb_layers; /*!< number of layers*/ 
                        unsigned input_dim; /*!< dimention of the input xt (dim of the embedding)*/
                        unsigned hidden_dim; /*!< dimention of the hidden states ht and  ct*/ 
                        float dropout_rate; /*!< dropout rate (between 0 and 1)*/ 
                        bool apply_dropout = true; /*!< apllying dropout or not*/ 

                /*Methods*/
                        void words_representation(Embeddings& embedding, Data& set, unsigned sentence,
                                dynet::ComputationGraph& cg, unsigned num_sentence, std::vector<dynet::Expression>& sentence_repr);

                        dynet::Expression run_KIM(Data& set, Embeddings& embedding, unsigned num_sentence, 
                                dynet::ComputationGraph& cg);

                        void create_attention_matrix(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& matrix, 
                                std::vector<dynet::Expression>& premise_lstm_repr, std::vector<dynet::Expression>& hypothesis_lstm_repr);

                        void compute_beta(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& beta_matrix, 
                                std::vector< std::vector<float> >& matrix);

                        void compute_alpha(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& alpha_matrix, 
                                std::vector< std::vector<float> >& matrix);

                        void compute_a_context_vector(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& alpha_matrix,
                                std::vector<dynet::Expression>& hypothesis_lstm_repr, unsigned premise_size, std::vector<dynet::Expression>& a_c_vect);

                        void compute_b_context_vector(dynet::ComputationGraph& cg, std::vector< std::vector<float> >& beta_matrix,
                                std::vector<dynet::Expression>& premise_lstm_repr, unsigned hypothesis_size, std::vector<dynet::Expression>& b_c_vect);

                        unsigned predict(Data& set, Embeddings& embedding, unsigned num_sentence, 
                                dynet::ComputationGraph& cg);

                        void train_score(Data& train_set, Embeddings& embedding, unsigned nb_batches, 
                                unsigned& nb_samples, unsigned batch_size, unsigned completed_epoch, unsigned nb_of_sentences, dynet::Trainer* trainer, std::vector<unsigned>& order, unsigned& numero_sentence);

                        void dev_score(dynet::ParameterCollection& model, Data& dev_set, Embeddings& embedding, std::string parameter_filename, unsigned nb_of_sentences_dev, double& best, char* output_emb_filename);

                        dynet::Expression get_neg_log_softmax(Data& set, Embeddings& embedding, unsigned num_sentence, 
                                dynet::ComputationGraph& cg);

                        void disable_dropout();

                        void enable_dropout();

                public:
                /*Methods*/
                        BiLSTM(unsigned nblayer, unsigned inputdim, 
                                unsigned hiddendim, float dropout, dynet::ParameterCollection& model);

                        void run_predict(dynet::ParameterCollection& model, Data& test_set, Embeddings& embedding, char* parameters_filename);

                        void run_train(dynet::ParameterCollection& model, Data& train_set, Data& dev_set, 
                                Embeddings& embedding, char* output_emb_filename, unsigned nb_epoch, unsigned batch_size);

        };

#endif