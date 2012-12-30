/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef CLIENTTOSLAVEREPLY_H
#define CLIENTTOSLAVEREPLY_H

#include <vector>
#include <utility>

#include "Post.hxx"
#include "../shared/Request.hxx"

class ClientToSlaveRequest;

/**
 * Odpowiedź na żądanie klienta do Slave.
 * Przechowuje aktualizacje dla klienta oraz informacje o zatwierdzonych
 * aktualizacjach wysłanych przez klienta.
 */
class ClientToSlaveReply
{
public:
    ClientToSlaveReply();

    /**
     * @return odpowiedź Slave w postaci obiektu
     */
    static ClientToSlaveReply receiveFrom(boost::asio::ip::tcp::socket &socket,
                                          const ClientToSlaveRequest &request);

    /**
     * Dodaje informacje o przydzielonym ID dla wiadomości wysłanej przez
     * klienta (0 jeżeli nie udało się)
     *
     * @param id ID nadane kolejnej wiadomości przesłanej przez klienta
     */
    void addCommitedPostId(PostId id);

    /**
     * Dodaje aktualizacje dla kolejnej dyskusji do zadanej wersji która
     * pociąga za sobą przesłanie listy nowych wiadomości. Jeżeli
     * wiadomość pochodzi ad klienta (jej ID istnieje na liście ID
     * przydzielonych przez slave dla wiadomości wysłanej przez klienta) -
     * treść wiadomości jest pomijana.
     *
     * @param version wersja do której wysyłana jest aktualizacja dla
     *          danej dyskusji
     * @param update lista nowych wiadomości które są przesyłane w ramach
     *          aktualizacji
     */
    void addPreparedUpdate(DiscussionVersion version,const std::vector<std::pair<PostId,Post>> &update);

    /**
     * @overload addPreparedUpdate(DiscussionVersion,const std::vector<std::pair<PostID,Post>> &)
     */
    void addPreparedUpdate(DiscussionVersion version,std::vector<std::pair<PostId,Post>> &&update);

    /**
     * @return lista aktualizacji kolejnych dyskusji z żądania
     */
    const std::vector<std::pair<DiscussionVersion,
          std::vector<std::pair<PostId,Post>>>> & updates() const;

    /**
     * @return Lista przydzielonych ID dla kolejnych wiadomości przesłanych
     * przez klienta
     */
    const  std::vector<PostId>& commited() const;

    /**
     * Wysyła odpowiedź
     *
     * @param socket gniazdo do którego zostanie wysłana adpowiedź
     */
    void sendTo(boost::asio::ip::tcp::socket& socket, const ClientToSlaveRequest& request);

    static void checkUpdate(const std::vector<std::pair<PostId,Post>> &update);

private:
    std::vector<std::pair<DiscussionVersion,std::vector<std::pair<PostId,Post>>>> _updates;
    std::vector<PostId> _commited;

};

#endif // CLIENTTOSLAVEREPLY_H
